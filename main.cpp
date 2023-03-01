#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/qqmlextensionplugin.h>

#include <FilesListModule/FilesList.h>
#include <CompressorLib/Compressor.h>
#include <thread>

int main(int argc, char *argv[])
{	
	Q_IMPORT_QML_PLUGIN(FilesListPlugin)

	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine;

	auto pathArg = argv[1];
	if (!pathArg) {
		pathArg = argv[0];
	}

	std::filesystem::path path(pathArg);

	Compressor::BMPCompressor comp;
	FilesList list(path);
	auto explorer = list.explorer();

	engine.rootContext()->setContextProperty("compressor", &comp);
	engine.rootContext()->setContextProperty("fileslist", &list);

	const QUrl url(u"qrc:/BMPCompressor/qml/main.qml"_qs);

	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
	                 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);

	engine.addImportPath(":/FilesListModule");
	engine.load(url);

	QObject::connect(&list,
	                 &FilesList::compressionRequested,
	                 &app,
	                 [&explorer, &comp]
	                 (const std::filesystem::path &path) {

		std::thread compress_t([&path, &comp] () {
			comp.compress(path);
		});
		compress_t.join();
	});

	QObject::connect(&list,
	                 &FilesList::decompressionRequested,
	                 &app,
	                 [&explorer, &comp]
	                 (const std::filesystem::path &path) {
		std::thread compress_t([&path, &comp] () {
			comp.decompress(path);
		});
		compress_t.join();
	});

	QObject::connect(&comp, &Compressor::BMPCompressor::compressionFinished, &list, &FilesList::compressionFinished);
	QObject::connect(&comp, &Compressor::BMPCompressor::decompressionFinished, &list, &FilesList::decompressionFinished);

	return app.exec();
}
