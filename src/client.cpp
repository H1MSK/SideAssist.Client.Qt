#include "client.hpp"
#include <QReadLocker>
#include <QWriteLocker>

namespace SideAssist::Qt {

Client::Client(const QHostAddress& host, const quint16 port, QObject* parent)
    : mqtt_client_(std::make_unique<QMQTT::Client>(host, port, parent)) {
  connectSignals();
}

Client::Client(const QString& hostName,
               const quint16 port,
               const QSslConfiguration& config,
               const bool ignoreSelfSigned,
               QObject* parent)
    : mqtt_client_(std::make_unique<QMQTT::Client>(hostName,
                                                   port,
                                                   config,
                                                   ignoreSelfSigned,
                                                   parent)) {
  connectSignals();
}

Client::Client(const QString& url,
               const QString& origin,
               QWebSocketProtocol::Version version,
               bool ignoreSelfSigned,
               QObject* parent)
    : mqtt_client_(std::make_unique<QMQTT::Client>(url,
                                                   origin,
                                                   version,
                                                   ignoreSelfSigned,
                                                   parent)) {
  connectSignals();
}

Client::Client(const QString& url,
               const QString& origin,
               QWebSocketProtocol::Version version,
               const QSslConfiguration& config,
               const bool ignoreSelfSigned,
               QObject* parent)
    : mqtt_client_(std::make_unique<QMQTT::Client>(url,
                                                   origin,
                                                   version,
                                                   config,
                                                   ignoreSelfSigned,
                                                   parent)) {
  connectSignals();
}

void Client::connectSignals() {
  connect(mqtt_client_.get(), &QMQTT::Client::connected, this,
          &Client::connected);
  connect(this, &Client::connected, this, &Client::logConnected);
  connect(mqtt_client_.get(), &QMQTT::Client::disconnected, this,
          &Client::disconnected);
  connect(this, &Client::disconnected, this, &Client::logDisconnected);

  connect(mqtt_client_.get(), &QMQTT::Client::subscribed, this,
          &Client::logSubscribed);
  connect(mqtt_client_.get(), &QMQTT::Client::unsubscribed, this,
          &Client::logUnsubscribed);
  connect(mqtt_client_.get(), &QMQTT::Client::published, this,
          &Client::logPublished);

  connect(mqtt_client_.get(), &QMQTT::Client::error, this,
          &Client::handleMqttError);

  connect(mqtt_client_.get(), &QMQTT::Client::connected, this,
          &Client::setupSubscriptions);
  connect(this, &Client::connected, this, &Client::uploadAll);
  connect(mqtt_client_.get(), &QMQTT::Client::received, this,
          &Client::handleMessage);
}

void Client::connectToHost() {
  mqtt_client_->setCleanSession(true);
  mqtt_client_->connectToHost();
}

void Client::setClientId(const QString& clientId) {
  mqtt_client_->setClientId(clientId);
}

void Client::setUsername(const QString& username) {
  mqtt_client_->setUsername(username);
}

void Client::setPassword(const QByteArray& password) {
  mqtt_client_->setPassword(password);
}

void Client::setupSubscriptions() {
  {
    QReadLocker lock(&options_lock_);
    for (auto& itr : options_) {
      setupSubscriptionsForOption(itr.second.get(), itr.second->value().isUndefined());
    }
  }
}

void Client::uploadAll() {
  {
    QReadLocker lock(&options_lock_);
    for (auto& itr : options_) {
      auto& ptr = itr.second;
      if (ptr->value().type() != QJsonValue::Undefined)
        uploadOptionValue(ptr.get());
      if (ptr->validator())
        uploadOptionValidator(ptr.get());
    }
  }
  {
    QReadLocker lock(&parameters_lock_);
    for (auto& itr : parameters_) {
      auto& ptr = itr.second;
      if (ptr->value().type() != QJsonValue::Undefined)
        uploadParameterValue(ptr.get());
      if (ptr->validator())
        uploadParameterValidator(ptr.get());
    }
  }
}

}  // namespace SideAssist::Qt
