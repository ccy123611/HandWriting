#ifndef TEXTPREDICTIONENGINE_H
#define TEXTPREDICTIONENGINE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QTextStream>

class TextPredictionEngine
{
public:
	explicit TextPredictionEngine(const QString& dbPath, const QString& txtFilePath = "");
	~TextPredictionEngine();

	void updatePriority(const QString& character);
	QVector<QString>predict(const QString& input)const;

private:
	QString m_dbPath;       //数据库文件路径
	QString m_txtFilePath;  //词组文本文件路径
	QSqlDatabase m_database;//SQLite 数据库连接

	void initializeDatabase(); //初始化数据库
	void ensureDatabaseExists();//确保数据库存在
	void importWordsFromFile(const QString& filePath);//从文本文件导入词组到数据库


};

#endif