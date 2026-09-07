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
    try
    {
        // 구조체에서 값을 받아와서 세션 생성
        m_session = std::make_unique<mysqlx::Session>(
            mysqlx::SessionOption::HOST, config.host,
            mysqlx::SessionOption::PORT, config.port,
            mysqlx::SessionOption::USER, config.user,
            mysqlx::SessionOption::PWD, config.password
        );

        // 데이터베이스 선택
        m_session->sql(
            "USE " + config.database
        ).execute();

        return true;
    }
    catch (const std::exception& e)
    {
        Logger::GetInstance().Error(
            e.what()
        );

        return false;
    }
}

bool Database::ExistsAccount(
    const std::string& loginId
)
{
    if (!m_session)
    {
        Logger::GetInstance().Error("Database session is not connected!");
        return false;
    }

    return true;
}

std::shared_ptr<Account> Database::LoadAccount(
    const std::string& loginId,
    const std::string& password,
    ErrorCode& error
)
{
    auto result =
        m_session
        ->sql(
            "SELECT BIN_TO_UUID(UUID), Loginid, PasswordHash, Salt "
            "FROM Users "
            "WHERE LoginId=?"
        )
        .bind(loginId)
        .execute();

    auto row =
        result.fetchOne();

    if (row.isNull())
    {
        error = ErrorCode::InvalidId;
        return nullptr;
    }

    // password SHA256 비교
    std::string storedHash =
        row[2].get<std::string>();

    std::string salt =
        row[3].get<std::string>();

    std::string inputHash =
        SHA256::Hash(password + salt);

    if (storedHash != inputHash)
    {
        error = ErrorCode::InvalidPassword;
        return nullptr;
    }

    auto account =
        std::make_shared<Account>();

    account->uuid =
        row[0].get<std::string>();

    account->loginId =
        row[1].get<std::string>();

    account->password =
        row[2].get<std::string>();

    error = ErrorCode::None;
    return account;
}

bool Database::InsertAccount(
    const std::string& loginId,
    const std::string& password,
    const std::string& salt,
    ErrorCode& error
)
{
    try
    {
        // Check
        auto check =
            m_session
            ->sql(
                "SELECT COUNT(*) "
                "FROM Users "
                "WHERE LoginId=?"
            )
            .bind(loginId)
            .execute();
        Logger::GetInstance().Info(loginId);

        auto row =
            check.fetchOne();


        if (!row.isNull() &&
            row[0].get<int>() > 0)
        {
            Logger::GetInstance()
                .Warning(
                    "Already Exists LoginId"
                );
            error = ErrorCode::DuplicateId;

            return false;
        }

        m_session
            ->sql(
                "INSERT INTO Users "
                "(LoginId, PasswordHash, Salt, UUID) "
                "VALUES (?, ?, ?, UUID_TO_BIN(UUID()))"
            )
            .bind(
                loginId,
                password,
                salt
            )
            .execute();

        error = ErrorCode::None;
    }
    catch (const mysqlx::Error& e)
    {
        Logger::GetInstance().Error(
            e.what()
        );

        return false;
    }

    auto result =
        m_session
        ->sql(
            "SELECT BIN_TO_UUID(UUID) "
            "FROM Users "
            "WHERE LoginId=?"
        )
        .bind(loginId)
        .execute();

    auto row =
        result.fetchOne();

    return true;
}