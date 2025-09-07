#include "TextPredictionEngine.h"
#include <QFile>
#include <QSqlQuery>
#include <QTextStream>
#include <QDir>


TextPredictionEngine::TextPredictionEngine(const QString& dbPath, const QString& txtFilePath)
	:m_dbPath(dbPath), m_txtFilePath(txtFilePath)
{
	qDebug() << "Database path: " << m_dbPath;
	//ȷ�����ݿ���ڣ�������������ʼ������������
	if (!QFile::exists(m_dbPath))
	{
		//�����ݿ�����
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

// ��ʼ�����ݿ�ṹ
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

//ȷ�����ݿ����
void TextPredictionEngine::ensureDatabaseExists()
{
	initializeDatabase();
}

// ���ı��ļ�������鵽���ݿ�
void TextPredictionEngine::importWordsFromFile(const QString& filePath) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "Failed to open file:" << filePath << " Error:" << file.errorString();
		return;
	}

	// ������ȷ�ı����ʽ��������������
	QTextStream in(&file);
	in.setCodec("UTF-8");

	// ȷ�� SQL �����ȷ
	QSqlQuery query;
	query.prepare("INSERT OR IGNORE INTO WordDictionary (word) VALUES (:word)");

	// ��ʼһ���������������
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

	// �ύ����
	m_database.commit();

	file.close();
}

// �����ַ������ȼ�
void TextPredictionEngine::updatePriority(const QString& character) {
	QSqlQuery query;

	// ���������� character Ϊǰ׺�Ĵ�������ȼ�
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

// ��������Ԥ�����
QVector<QString> TextPredictionEngine::predict(const QString& input) const {
	QVector<QString> predictions;

	// ȷ�����ݿ��Ѵ�
	if (!m_database.isOpen()) {
		qCritical() << "Database is not open!";
		return predictions;
	}

	// ������ѯ����
	QSqlQuery query;

	// ׼����ѯ
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

	// �󶨲���
	query.bindValue(":prefix", input + "%");
	qDebug() << "Bound Values: " << query.boundValues();  // ��ӡ�󶨵�ֵ

	// ִ�в�ѯ
	if (!query.exec()) {
		qCritical() << "Failed to execute query: " << query.lastError().text();
		return predictions;
	}

	// ��ȡ��ѯ���
	while (query.next()) {
		predictions.append(query.value(0).toString());
	}

	return predictions;
}