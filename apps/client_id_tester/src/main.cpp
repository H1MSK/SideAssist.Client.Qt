#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <memory>
#include "client.hpp"
#include "value_validator.hpp"

std::shared_ptr<SideAssist::Qt::Client> client;

int main(int argc, char* argv[]) {
  namespace Validator = SideAssist::Qt::ValueValidator;
  QCoreApplication app(argc, argv);
  auto args = app.arguments();

  if (args.length() != 2) {
    qCritical("Usage: %s client_id",
              qUtf8Printable(QFileInfo(args[0]).fileName()));
    exit(1);
  }

  client = std::make_shared<SideAssist::Qt::Client>();

  client->setClientId(args[1]);
  client->installDefaultMessageHandler();

  client->setUsername("side_assist_client");
  client->setPassword("16509490");

  client->connectToHost();

  auto int_opt = client->addOption("int");
  int_opt->setValidator(std::make_shared<Validator::SingleType>(
      Validator::ValueTypeFieldEnum::Integer));

  auto int_param = client->addParameter("int_param");
  QObject::connect(int_opt.get(), &SideAssist::Qt::NamedValue::valueChanged,
                   int_param.get(), &SideAssist::Qt::NamedValue::setValue);
  return app.exec();
}
