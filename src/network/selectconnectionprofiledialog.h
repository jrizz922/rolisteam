#ifndef SELECTCONNECTIONPROFILEDIALOG_H
#define SELECTCONNECTIONPROFILEDIALOG_H

#include <QDialog>
#include "data/player.h"
#include <QSettings>

#include <QAbstractListModel>

namespace Ui {
class SelectConnectionProfileDialog;
}

/**
 * @brief The ConnectionProfile class
 */
class ConnectionProfile
{
public:
    /**
     * @brief ConnectionProfile
     */
    ConnectionProfile();
    /**
     * @brief ~ConnectionProfile
     */
    virtual ~ConnectionProfile();
    /**
     * @brief setTitle
     */
    void setTitle(QString);
    /**
     * @brief setName
     */
    void setName(QString);
    /**
     * @brief setAddress
     */
    void setAddress(QString);
    /**
     * @brief setPort
     */
    void setPort(int);
    /**
     * @brief setServerMode
     */
    void setServerMode(bool);
    /**
     * @brief setPlayer
     */
    void setPlayer(Player*);
    /**
     * @brief setGm
     */
    void setGm(bool);


    /**
     * @brief getTitle
     * @return
     */
    QString getTitle()const;
    /**
     * @brief getName
     * @return
     */
    QString getName() const;
    /**
     * @brief getAddress
     * @return
     */
    QString getAddress() const;
    /**
     * @brief getPort
     * @return
     */
    int     getPort() const;
    /**
     * @brief isServer
     * @return
     */
    bool    isServer() const;
    /**
     * @brief getPlayer
     * @return
     */
    Player* getPlayer() const;
    /**
     * @brief isGM
     * @return
     */
    bool    isGM() const;



private:
    QString m_title;
    QString m_name;
    QString m_address;
    bool    m_server;
    int     m_port;
    Player*  m_player;
    bool    m_isGM;

};

/**
 * @brief The ProfileModel class
 */
class ProfileModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
    * @brief ProfileModel
    */
   ProfileModel();
   /**
    * @brief ~ProfileModel
    */
   virtual ~ProfileModel();
   /**
    * @brief rowCount
    * @param parent
    * @return
    */
   virtual int rowCount(const QModelIndex &parent) const;
   /**
    * @brief data
    * @param index
    * @param role
    * @return
    */
   virtual QVariant data(const QModelIndex &index, int role) const;
   /**
    * @brief headerData
    * @param section
    * @param orientation
    * @param role
    * @return
    */
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole);
   /**
   * Load informations from the previous rolisteam's execution
   */
   void readSettings(QSettings & settings);

   /**
   * Save parameters for next executions.
   */
   void writeSettings(QSettings & settings);

   Qt::ItemFlags flags(const QModelIndex & index);


   ConnectionProfile* getProfile(const QModelIndex&);

public slots:
   /**
    * @brief ProfileModel::appendProfile
    */
   void appendProfile();


private:
    QList<ConnectionProfile*> m_connectionProfileList;

};

/**
 * @brief The SelectConnectionProfileDialog class
 */
class SelectConnectionProfileDialog : public QDialog
{
	Q_OBJECT

public:
    /**
     * @brief SelectConnectionProfileDialog
     * @param parent
     */
	explicit SelectConnectionProfileDialog(QWidget *parent = 0);
    /**
     * @brief ~SelectConnectionProfileDialog
     */
    virtual	~SelectConnectionProfileDialog();

    /**
     * @brief getSelectedProfile
     * @return
     */
    ConnectionProfile* getSelectedProfile();

    /**
    * Load informations from the previous rolisteam's execution
    */
    void readSettings(QSettings & settings);

    /**
    * Save parameters for next executions.
    */
    void writeSettings(QSettings & settings);


    /**
     * @brief updateGUI
     */
    void updateGUI();
public slots:

    /**
     * @brief updateProfile
     */
    void updateProfile();
    /**
     * @brief setCurrentProfile
     */
    void setCurrentProfile(QModelIndex );

private:
	Ui::SelectConnectionProfileDialog *ui;
    ProfileModel* m_model;
    ConnectionProfile* m_currentProfile;
};

#endif // SELECTCONNECTIONPROFILEDIALOG_H
