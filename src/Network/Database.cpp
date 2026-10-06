#include "Network/Database.h"
#include "Core/Logger.h"
#include "Crypto/SHA256.h"
#include "Network/ErrorCode.h"

Database::Database()
{
}

Database::~Database() = default;

bool Database::Connect(const DBConfig& config)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;
    return ConnectSession();
}

bool Database::ConnectSession()
{
    m_session.reset();

    try
    {
        m_session = std::make_unique<mysqlx::Session>(
            mysqlx::SessionOption::HOST, m_config.host,
            mysqlx::SessionOption::PORT, m_config.port,
            mysqlx::SessionOption::USER, m_config.user,
            mysqlx::SessionOption::PWD, m_config.password
        );

        m_session->sql(
            "USE " + m_config.database
        ).execute();

        return true;
    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().Error(
            e.what()
        );

        m_session.reset();
        return false;
    }
}

bool Database::RunWithReconnect(
    const std::function<void(mysqlx::Session&)>& operation
)
{
    for (int attempt = 0; attempt < 2; ++attempt)
    {
        if (!m_session && !ConnectSession())
        {
            if (attempt == 1)
            {
                return false;
            }

            continue;
        }

        try
        {
            operation(*m_session);
            return true;
        }
        catch (const mysqlx::Error& e)
        {
            Logger::GetInstance().Warning(
                std::string("MySQL operation failed; reconnecting: ") + e.what()
            );
            m_session.reset();

            if (attempt == 1 || !ConnectSession())
            {
                return false;
            }
        }
    }

    return false;
}

bool Database::ExistsAccount(
    const std::string& loginId
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return RunWithReconnect(
        [](mysqlx::Session& session)
        {
            session.sql("SELECT 1").execute();
        }
    );
}

std::shared_ptr<Account> Database::LoadAccount(
    const std::string& loginId,
    const std::string& password,
    ErrorCode& error
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::shared_ptr<Account> account;
    const bool succeeded = RunWithReconnect(
        [&](mysqlx::Session& session)
        {
            auto result =
                session
                .sql(
                    "SELECT BIN_TO_UUID(UUID), Loginid, PasswordHash, Salt "
                    "FROM Users "
                    "WHERE LoginId=?"
                )
                .bind(loginId)
                .execute();

            auto row = result.fetchOne();
            if (row.isNull())
            {
                error = ErrorCode::InvalidId;
                return;
            }

            std::string storedHash = row[2].get<std::string>();
            std::string salt = row[3].get<std::string>();
            if (storedHash != SHA256::Hash(password + salt))
            {
                error = ErrorCode::InvalidPassword;
                return;
            }

            account = std::make_shared<Account>();
            account->uuid = row[0].get<std::string>();
            account->loginId = row[1].get<std::string>();
            account->password = storedHash;
            account->salt = salt;
            error = ErrorCode::None;
        }
    );

    if (!succeeded)
    {
        error = ErrorCode::InternalServerError;
        return nullptr;
    }
    return account;
}

bool Database::InsertAccount(
    const std::string& loginId,
    const std::string& password,
    const std::string& salt,
    ErrorCode& error
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    bool alreadyExists = false;
    if (!RunWithReconnect(
        [&](mysqlx::Session& session)
        {
            auto check =
                session
                .sql(
                    "SELECT COUNT(*) "
                    "FROM Users "
                    "WHERE LoginId=?"
                )
                .bind(loginId)
                .execute();

            auto row = check.fetchOne();
            alreadyExists = !row.isNull() && row[0].get<int>() > 0;
        }
    ))
    {
        error = ErrorCode::InternalServerError;
        return false;
    }

    if (alreadyExists)
    {
        Logger::GetInstance().Warning("Already Exists LoginId");
        error = ErrorCode::DuplicateId;
        return false;
    }

    const bool inserted = RunWithReconnect(
        [&](mysqlx::Session& session)
        {
            session
                .sql(
                    "INSERT INTO Users "
                    "(LoginId, PasswordHash, Salt, UUID) "
                    "VALUES (?, ?, ?, UUID_TO_BIN(UUID()))"
                )
                .bind(loginId, password, salt)
                .execute();
        }
    );

    if (!inserted)
    {
        error = ErrorCode::InternalServerError;
        return false;
    }

    error = ErrorCode::None;
    return true;
}