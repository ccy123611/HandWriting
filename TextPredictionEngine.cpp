#include "TextPredictionEngine.h"
#include <QFile>
#include <QSqlQuery>
#include <QTextStream>
#include <QDir>


TextPredictionEngine::TextPredictionEngine(const QString& dbPath, const QString& txtFilePath)
	:m_dbPath(dbPath), m_txtFilePath(txtFilePath)
{
	qDebug() << "Database path: " << m_dbPath;
	//确保数据库存在，如果不存在则初始化并导入数据
	if (!QFile::exists(m_dbPath))
	{
		//打开数据库连接
		m_database = QSqlDatabase::addDatabase("QSQLITE");
		m_database.setDatabaseName(m_dbPath);
		if (!m_database.open())
		{
			qCritical() << "Failed to open database:" << m_database.lastError().text();
			return;
		}
		ensureDatabaseExists();
		if (!m_txtFilePath.isEmpty())
		{
			importWordsFromFile(m_txtFilePath);
		}
	}
	else
	{
		m_database = QSqlDatabase::addDatabase("QSQLITE");
		m_database.setDatabaseName(m_dbPath);
		if (!m_database.open())
		{

			qCritical() << "Failed to open database:" << m_database.lastError().text();
			return;
		}
		
		if (!m_txtFilePath.isEmpty())
		{
			importWordsFromFile(m_txtFilePath);
		}
		return;
	}
}

TextPredictionEngine::~TextPredictionEngine()
{
	if (m_database.isOpen())
	{
		m_database.close();
	}
}

// 初始化数据库结构
void TextPredictionEngine::initializeDatabase() {
	QSqlQuery query;
	QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS WordDictionary (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            word TEXT UNIQUE,
            priority INTEGER DEFAULT 0
        )
    )";

	if (!query.exec(createTableQuery)) {
		qCritical() << "Failed to create table:" << query.lastError().text();
	}
}

//确保数据库存在
void TextPredictionEngine::ensureDatabaseExists()
{
	initializeDatabase();
}

// 从文本文件导入词组到数据库
void TextPredictionEngine::importWordsFromFile(const QString& filePath) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "Failed to open file:" << filePath << " Error:" << file.errorString();
		return;
	}

	// 设置正确的编码格式，避免中文乱码
	QTextStream in(&file);
	in.setCodec("UTF-8");

	// 确保 SQL 语句正确
	QSqlQuery query;
	query.prepare("INSERT OR IGNORE INTO WordDictionary (word) VALUES (:word)");

	// 开始一个事务，以提高性能
	m_database.transaction();

	while (!in.atEnd()) {
		QString line = in.readLine().trimmed();
		if (!line.isEmpty()) {
			query.bindValue(":word", line);
			if (!query.exec()) {
				qCritical() << "Failed to insert word:" << query.lastError().text();
			}
		}
	}

	// 提交事务
	m_database.commit();

	file.close();
}

// 更新字符的优先级
void TextPredictionEngine::updatePriority(const QString& character) {
	QSqlQuery query;

	// 更新所有以 character 为前缀的词组的优先级
	query.prepare(R"(
        UPDATE WordDictionary
        SET priority = priority + 1
        WHERE word LIKE :prefix
    )");
	query.bindValue(":prefix", character + "%");

	if (!query.exec()) {
		qCritical() << "Failed to update priority:" << query.lastError().text();
	}
}

// 根据输入预测词组
QVector<QString> TextPredictionEngine::predict(const QString& input) const {
	QVector<QString> predictions;

	// 确保数据库已打开
	if (!m_database.isOpen()) {
		qCritical() << "Database is not open!";
		return predictions;
	}

	// 创建查询对象
	QSqlQuery query;

	// 准备查询
	QString queryStr = R"(
        SELECT word
        FROM WordDictionary
        WHERE word LIKE :prefix
        AND LENGTH(word) <= 3
        ORDER BY priority DESC, word ASC
        LIMIT 4
    )";
	if (!query.prepare(queryStr)) {
		qCritical() << "Failed to prepare query:" << query.lastError().text();
		return predictions;
	}

	// 绑定参数
	query.bindValue(":prefix", input + "%");
	qDebug() << "Bound Values: " << query.boundValues();  // 打印绑定的值

	// 执行查询
	if (!query.exec()) {
		qCritical() << "Failed to execute query: " << query.lastError().text();
		return predictions;
	}

	// 获取查询结果
	while (query.next()) {
		predictions.append(query.value(0).toString());
	}

	return predictions;
}