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
	QString m_dbPath;       //���ݿ��ļ�·��
	QString m_txtFilePath;  //�����ı��ļ�·��
	QSqlDatabase m_database;//SQLite ���ݿ�����

	void initializeDatabase(); //��ʼ�����ݿ�
	void ensureDatabaseExists();//ȷ�����ݿ����
	void importWordsFromFile(const QString& filePath);//���ı��ļ�������鵽���ݿ�


};

#endif