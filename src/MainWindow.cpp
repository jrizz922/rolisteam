/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QtGui>
#include <QDebug>


#include "MapFrame.h"
#include "Carte.h"
#include "MainWindow.h"
#include "BarreOutils.h"
#include "ListeUtilisateurs.h"
#include "constantesGlobales.h"
#include "variablesGlobales.h"

#include "Image.h"
#include "LecteurAudio.h"
#include "EditeurNotes.h"
#include "WorkspaceAmeliore.h"

#include "preferencesmanager.h"

MainWindow::MainWindow()
        : QMainWindow()
{
        m_options = PreferencesManager::getInstance();
        readSettings();



        // Initialisation de la liste des CarteFenetre, des Image et des Tchat
        listeCarteFenetre.clear();
        listeImage.clear();
        listeTchat.clear();



        // Desactivation des animations
        setAnimated(false);
        // Creation de l'espace de travail
        workspace = new ImprovedWorkspace;
        // Ajout de l'espace de travail dans la fenetre principale
        setCentralWidget(workspace);
        // Connexion du changement de fenetre active avec la fonction de m.a.j du selecteur de taille des PJ
        //connect(workspace, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(changementFenetreActive(QMdiSubWindow *)));

        // Creation de la barre d'outils
        barreOutils = new ToolsBar(this);
        // Ajout de la barre d'outils a la fenetre principale
        addDockWidget(Qt::LeftDockWidgetArea, barreOutils);


        connect(barreOutils,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),workspace,SLOT(currentToolChanged(ToolsBar::SelectableTool)));

        // Creation du log utilisateur
        dockLogUtil = creerLogUtilisateur();
        // Ajout du log utilisateur a la fenetre principale
        addDockWidget(Qt::RightDockWidgetArea, dockLogUtil);





        m_playerListDockWidget = new UserListDockWidget;
        addDockWidget(Qt::RightDockWidgetArea, m_playerListDockWidget);


        m_audioPlayer = LecteurAudio::getInstance(this);

        addDockWidget(Qt::RightDockWidgetArea, m_audioPlayer);


        creerMenu();
        // Association des actions des menus avec des fonctions
        associerActionsMenus();
        // Autoriser/interdire action en fonction de la nature de l'utilisateur (joueur ou MJ)
        autoriserOuInterdireActions();

        // Creation de l'editeur de notes
        editeurNotes = new EditeurNotes();
        // Ajout de l'editeur de notes au workspace
        workspace->addWidget(editeurNotes);
        // Mise a jour du titre de l'editeur de notes
        editeurNotes->setWindowTitle(tr("Editeur de notes"));
        // Masquage de l'editeur de notes
        editeurNotes->hide();

        // Initialisation des etats de sante des PJ/PNJ (variable declarees dans DessinPerso.cpp)
        DessinPerso::etatDeSante etat;
        etat.couleurEtat = Qt::black;
        etat.nomEtat = tr("Sain");
        G_etatsDeSante.append(etat);

        etat.couleurEtat = QColor(255, 100, 100);
        etat.nomEtat = tr("Blessé léger");
        G_etatsDeSante.append(etat);

        etat.couleurEtat = QColor(255, 0, 0);
        etat.nomEtat = tr("Blessé grave");
        G_etatsDeSante.append(etat);

        etat.couleurEtat = Qt::gray;
        etat.nomEtat = tr("Mort");
        G_etatsDeSante.append(etat);

        etat.couleurEtat = QColor(80, 80, 255);
        etat.nomEtat = tr("Endormi");
        G_etatsDeSante.append(etat);

        etat.couleurEtat = QColor(0, 200, 0);
        etat.nomEtat = tr("Ensorcelé");
        G_etatsDeSante.append(etat);









}

/********************************************************************/
/* Creation du log utilisateur                                      */
/********************************************************************/
QDockWidget* MainWindow::creerLogUtilisateur()
{
        // Creation du dockWidget contenant la fenetre de log utilisateur
        QDockWidget *dockLogUtil = new QDockWidget(tr("Evènements"), this);
dockLogUtil->setAllowedAreas(Qt::AllDockWidgetAreas);
        dockLogUtil->setFeatures(QDockWidget::AllDockWidgetFeatures);

        //Creation du log utilisateur
        /*logUtilisateur = new QTextEdit(dockLogUtil);
        logUtilisateur->setReadOnly(true);*/

        // Insertion de la fenetre de log utilisateur dans le dockWidget
        //dockLogUtil->setWidget(logUtilisateur);
        // Largeur minimum du log utilisateur
        dockLogUtil->setMinimumWidth(125);

        return dockLogUtil;
}

/********************************************************************/
/* Creation de la barre de menus, des menus, des actions associees  */
/********************************************************************/
void MainWindow::creerMenu()
{

        QMenuBar *barreMenus = new QMenuBar(this);

        setMenuBar(barreMenus);

        // Creation du menu Fichier
        QMenu *menuFichier = new QMenu (tr("Fichier"), barreMenus);
        newMapAction            = menuFichier->addAction(tr("&New empty map"));
        menuFichier->addSeparator();
        actionOuvrirPlan             = menuFichier->addAction(tr("Ouvrir plan"));
        actionOuvrirEtMasquerPlan    = menuFichier->addAction(tr("Ouvrir et masquer plan"));
        actionOuvrirScenario         = menuFichier->addAction(tr("Ouvrir scénario"));
        actionOuvrirImage	         = menuFichier->addAction(tr("Ouvrir image"));
        actionOuvrirNotes            = menuFichier->addAction(tr("Ouvrir notes"));
        menuFichier->addSeparator();
        actionFermerPlan             = menuFichier->addAction(tr("Fermer plan/image"));
        menuFichier->addSeparator();
        actionSauvegarderPlan        = menuFichier->addAction(tr("Sauvegarder plan"));
        actionSauvegarderScenario    = menuFichier->addAction(tr("Sauvegarder scénario"));
        actionSauvegarderNotes       = menuFichier->addAction(tr("Sauvegarder notes"));
/*
        menuFichier->addSeparator();
        actionPreferences            = menuFichier->addAction(tr("Préfrences"));
*/
        menuFichier->addSeparator();
        actionQuitter                = menuFichier->addAction(tr("Quitter"));

        // Creation du menu Affichage
        QMenu *menuAffichage = new QMenu (tr("Affichage"), barreMenus);
        actionAfficherNomsPj         = menuAffichage->addAction(tr("Afficher noms PJ"));
        actionAfficherNomsPnj        = menuAffichage->addAction(tr("Afficher noms PNJ"));
        actionAfficherNumerosPnj     = menuAffichage->addAction(tr("Afficher numros PNJ"));
/*
        // Creation du sous-menu Grille
        QMenu *sousMenuGrille = new QMenu (tr("Grille"), barreMenus);
        actionSansGrille             = sousMenuGrille->addAction(tr("Aucune"));
        actionCarre                  = sousMenuGrille->addAction(tr("Carrs"));
        actionHexagones              = sousMenuGrille->addAction(tr("Hexagones"));
        // Ajout du sous-menu Grille au menu Affichage
        menuAffichage->addSeparator();
        menuAffichage->addMenu(sousMenuGrille);
        // Creation du groupe d'actions pour le menu Grille
        QActionGroup *groupeAction = new QActionGroup(barreMenus);
        groupeAction->addAction(actionSansGrille);
        groupeAction->addAction(actionCarre);
        groupeAction->addAction(actionHexagones);
*/

        // Actions checkables
        actionAfficherNomsPj    ->setCheckable(true);
        actionAfficherNomsPnj   ->setCheckable(true);
        actionAfficherNumerosPnj->setCheckable(true);
/*
        actionSansGrille        ->setCheckable(true);
        actionCarre             ->setCheckable(true);
        actionHexagones         ->setCheckable(true);
*/

        // Choix des actions selectionnees au depart
        actionAfficherNomsPj    ->setChecked(true);
        actionAfficherNomsPnj   ->setChecked(true);
        actionAfficherNumerosPnj->setChecked(true);
/*
        actionSansGrille        ->setChecked(true);
*/
        // Creation du menu Fenetre
        menuFenetre = new QMenu (tr("Fenêtre"), barreMenus);

        // Creation du sous-menu Reorganiser
        QMenu *sousMenuReorganise    = new QMenu (tr("Réorganiser"), barreMenus);
        actionCascade                = sousMenuReorganise->addAction(tr("Cascade"));
        actionTuiles                 = sousMenuReorganise->addAction(tr("Tuiles"));
        // Ajout du sous-menu Reorganiser au menu Fenetre
        menuFenetre->addMenu(sousMenuReorganise);
        menuFenetre->addSeparator();

        // Ajout des actions d'affichage des fenetres d'evenement, utilisateurs et lecteur audio
        menuFenetre->addAction(dockLogUtil->toggleViewAction());
        //menuFenetre->addAction(G_listeUtilisateurs->toggleViewAction());
        menuFenetre->addAction(m_audioPlayer->toggleViewAction());
        menuFenetre->addSeparator();

        // Ajout de l'action d'affichage de l'editeur de notes
        actionEditeurNotes = menuFenetre->addAction(tr("Editeur de notes"));
        actionEditeurNotes->setCheckable(true);
        actionEditeurNotes->setChecked(false);
        // Connexion de l'action avec l'affichage/masquage de l'editeur de notes
        QObject::connect(actionEditeurNotes, SIGNAL(triggered(bool)), this, SLOT(afficherEditeurNotes(bool)));

        // Ajout du sous-menu Tchat
        sousMenuTchat = new QMenu (tr("Tchats"), barreMenus);
        menuFenetre->addMenu(sousMenuTchat);

        // Ajout de l'action d'affichage de la fenetre de tchat commun
        actionTchatCommun = sousMenuTchat->addAction(tr("Tchat commun"));
        actionTchatCommun->setCheckable(true);
        actionTchatCommun->setChecked(false);
        menuFenetre->addSeparator();

        // Creation du tchat commun
        listeTchat.append(new Tchat("", actionTchatCommun,NULL));
        // Ajout du tchat commun au workspace
        workspace->addWidget(listeTchat[0]);
        // Mise a jour du titre du tchat commun
        listeTchat[0]->setWindowTitle(tr("Tchat commun"));
        // Masquage du tchat commun
        listeTchat[0]->hide();
        // Connexion de l'action avec l'affichage/masquage du tchat commun
        QObject::connect(actionTchatCommun, SIGNAL(triggered(bool)), listeTchat[0], SLOT(setVisible(bool)));

        // Creation du menu Aide
        QMenu *menuAide = new QMenu (tr("Aide"), barreMenus);
        actionAideLogiciel = menuAide->addAction(tr("Aide sur") + " " + APPLICATION_NAME);
        menuAide->addSeparator();
        actionAPropos = menuAide->addAction(tr("A propos de") + " " + APPLICATION_NAME);

        // Ajout des menus a la barre de menus
        barreMenus->addMenu(menuFichier);
        barreMenus->addMenu(menuAffichage);
        barreMenus->addMenu(menuFenetre);
        barreMenus->addMenu(menuAide);
}

/********************************************************************/
/* Association des actions des menus avec des fonctions             */
/********************************************************************/
void MainWindow::associerActionsMenus()
{
        // file menu
        QObject::connect(newMapAction, SIGNAL(triggered(bool)), this, SLOT(clickOnMapWizzard()));
      /*  QObject::connect(actionOuvrirImage, SIGNAL(triggered(bool)), this, SLOT(ouvrirImage()));
        QObject::connect(actionOuvrirPlan, SIGNAL(triggered(bool)), this, SLOT(ouvrirPlan()));
        QObject::connect(actionOuvrirEtMasquerPlan, SIGNAL(triggered(bool)), this, SLOT(ouvrirEtMasquerPlan()));
        QObject::connect(actionOuvrirScenario, SIGNAL(triggered(bool)), this, SLOT(ouvrirScenario()));
        QObject::connect(actionOuvrirNotes, SIGNAL(triggered(bool)), this, SLOT(ouvrirNotes()));
        QObject::connect(actionFermerPlan, SIGNAL(triggered(bool)), this, SLOT(fermerPlanOuImage()));
        QObject::connect(actionSauvegarderPlan, SIGNAL(triggered(bool)), this, SLOT(sauvegarderPlan()));
        QObject::connect(actionSauvegarderScenario, SIGNAL(triggered(bool)), this, SLOT(sauvegarderScenario()));
        QObject::connect(actionSauvegarderNotes, SIGNAL(triggered(bool)), this, SLOT(sauvegarderNotes()));

        // close
        QObject::connect(actionQuitter, SIGNAL(triggered(bool)), this, SLOT(quitterApplication()));

        // Windows managing
        QObject::connect(actionCascade, SIGNAL(triggered(bool)), workspace, SLOT(cascadeSubWindows()));
        QObject::connect(actionTuiles, SIGNAL(triggered(bool)), workspace, SLOT(tileSubWindows()));

        // Display
        QObject::connect(actionAfficherNomsPj, SIGNAL(triggered(bool)), this, SLOT(afficherNomsPj(bool)));
        QObject::connect(actionAfficherNomsPnj, SIGNAL(triggered(bool)), this, SLOT(afficherNomsPnj(bool)));
        QObject::connect(actionAfficherNumerosPnj, SIGNAL(triggered(bool)), this, SLOT(afficherNumerosPnj(bool)));
*/
        // Help
        QObject::connect(actionAPropos, SIGNAL(triggered()), this, SLOT(about()));
        QObject::connect(actionAideLogiciel, SIGNAL(triggered()), this, SLOT(aideEnLigne()));
}

/********************************************************************/
/* Autorise ou interdit certains menus selon que l'utilisateur est  */
/* MJ ou joueur                                                     */
/********************************************************************/
void MainWindow::autoriserOuInterdireActions()
{
        // L'utilisateur est un joueur
     /*   if (G_joueur)
        {
                actionNouveauPlan->setEnabled(false);
                actionOuvrirPlan->setEnabled(false);
                actionOuvrirEtMasquerPlan->setEnabled(false);
                actionOuvrirScenario->setEnabled(false);
                actionFermerPlan->setEnabled(false);
                actionSauvegarderPlan->setEnabled(false);
                actionSauvegarderScenario->setEnabled(false);
        }*/

        // L'utilisateur est un MJ
       /* else
        {
        }*/
}

/********************************************************************/
/* Creation d'une nouvelle carte dans le workspace                  */
/********************************************************************/
/*void MainWindow::ajouterCarte(MapFrame *mapFrame, QString titre)
{
        // Ajout de la CarteFenetre a la liste (permet par la suite de parcourir l'ensemble des cartes)
        listeCarteFenetre.append(mapFrame);

        // Ajout de la carte au workspace
        workspace->addWidget(mapFrame);



        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);



        // Connexion de l'action avec l'affichage/masquage de la fenetre
        QObject::connect(action, SIGNAL(triggered(bool)), mapFrame, SLOT(setVisible(bool)));

        // Recuperation de la Carte contenue dans la CarteFenetre
        Map *map = mapFrame->map();

        // Connexion des actions de changement d'outil avec les fonctions de changement de pointeur de souris
        QObject::connect(barreOutils->actionCrayon,     SIGNAL(triggered(bool)), map, SLOT(drawingCursor()));
        QObject::connect(barreOutils->actionLigne,      SIGNAL(triggered(bool)), map, SLOT(drawingCursor()));
        QObject::connect(barreOutils->actionRectVide,   SIGNAL(triggered(bool)), map, SLOT(drawingCursor()));
        QObject::connect(barreOutils->actionRectPlein,  SIGNAL(triggered(bool)), map, SLOT(drawingCursor()));
        QObject::connect(barreOutils->actionElliVide,   SIGNAL(triggered(bool)), map, SLOT(drawingCursor()));
        QObject::connect(barreOutils->actionElliPlein,  SIGNAL(triggered(bool)), map, SLOT(drawingCursor()));
        QObject::connect(barreOutils->actionTexte,      SIGNAL(triggered(bool)), map, SLOT(pointeurTexte()));
        QObject::connect(barreOutils->actionMain,	    SIGNAL(triggered(bool)), map, SLOT(pointeurMain()));
        QObject::connect(barreOutils->actionAjoutPnj,   SIGNAL(triggered(bool)), map, SLOT(pointeurAjoutPnj()));
        QObject::connect(barreOutils->actionSupprPnj,   SIGNAL(triggered(bool)), map, SLOT(pointeurSupprPnj()));
        QObject::connect(barreOutils->actionDeplacePnj, SIGNAL(triggered(bool)), map, SLOT(pointeurDeplacePnj()));
        QObject::connect(barreOutils->actionEtatPnj,    SIGNAL(triggered(bool)), map, SLOT(pointeurEtatPnj()));

        // Connexion de la demande de changement de couleur de la carte avec celle de la barre d'outils
        QObject::connect(map, SIGNAL(changeCouleurActuelle(QColor)), barreOutils, SLOT(changeCouleurActuelle(QColor)));
        // Connexion de la demande d'incrementation du numero de PNJ de la carte avec celle de la barre d'outils
        QObject::connect(map, SIGNAL(incrementeNumeroPnj()), barreOutils, SLOT(incrementeNumeroPnj()));
        // Connexion de la demande de changement de diametre des PNJ de la carte avec celle de la barre d'outils
        QObject::connect(map, SIGNAL(mettreAJourPnj(int, QString)), barreOutils, SLOT(mettreAJourPnj(int, QString)));
        // Affichage des noms et numeros des PJ/PNJ
        QObject::connect(actionAfficherNomsPj, SIGNAL(triggered(bool)), map, SIGNAL(afficherNomsPj(bool)));
        QObject::connect(actionAfficherNomsPnj, SIGNAL(triggered(bool)), map, SIGNAL(afficherNomsPnj(bool)));
        QObject::connect(actionAfficherNumerosPnj, SIGNAL(triggered(bool)), map, SIGNAL(afficherNumerosPnj(bool)));
        // Creation d'un DessinPerso a la creation d'un nouveau personnage dans la liste
     /*   QObject::connect(G_listeUtilisateurs, SIGNAL(ajouterPj(QString , QString , QColor)), carte, SLOT(ajouterPj(QString , QString , QColor)));
        // Changement de nom d'un PJ
        QObject::connect(G_listeUtilisateurs, SIGNAL(renommerPj(QString , QString)), carte, SLOT(renommerPj(QString , QString)));
        // Suppression d'un PJ
        QObject::connect(G_listeUtilisateurs, SIGNAL(effacerPj(QString)), carte, SLOT(effacerPerso(QString)));
        // Changement de couleur d'un PJ
        QObject::connect(G_listeUtilisateurs, SIGNAL(changerCouleurPerso(QString, QColor)), carte, SLOT(changerCouleurPj(QString, QColor)));

        // Mise a jour du pointeur de souris de la carte
        switch(G_outilCourant)
        {
                case BarreOutils::crayon :
                case BarreOutils::ligne :
                case BarreOutils::rectVide :
                case BarreOutils::rectPlein :
                case BarreOutils::elliVide :
                case BarreOutils::elliPlein :
                        map->drawingCursor();
                        break;
                case BarreOutils::texte :
                        map->pointeurTexte();
                        break;
                case BarreOutils::main :
                        map->pointeurMain();
                        break;
                case BarreOutils::ajoutPnj :
                        map->pointeurAjoutPnj();
                        break;
                case BarreOutils::supprPnj :
                        map->pointeurSupprPnj();
                        break;
                case BarreOutils::deplacePerso :
                        map->pointeurDeplacePnj();
                        break;
                case BarreOutils::etatPerso :
                        map->pointeurEtatPnj();
                        break;
                default :
                        qWarning("Outil non dfini a la creation d'une fentre (ajouterCarte - MainWindow.cpp)");
                        break;
        }

        // Ajout dans la carte des PJ deja presents
        //G_listeUtilisateurs->ajouterTousLesPj(carte);
        // Affichage de la carte
        mapFrame->show();
}*/

/********************************************************************/
/* Ajout de l'Image passee en parametre sans le workspace           */
/********************************************************************/
/*void MainWindow::ajouterImage(Image *imageFenetre, QString titre)
{
        // Ajout de la  a la liste (permet par la suite de parcourir l'ensemble des cartes)
        listeImage.append(imageFenetre);

        // Ajout de la carte au workspace
        workspace->addWidget(imageFenetre);

        // Mise a jour du titre de la CarteFenetre
        imageFenetre->setWindowTitle(titre);

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

        // Association de l'action avec la carte
        imageFenetre->associerAction(action);

        // Connexion de l'action d'outil main avec la fonction de changement de pointeur de souris en main
        QObject::connect(barreOutils->actionMain,	    SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurMain()));

        // Connexion des actions de changement d'outil avec la fonction de changement de pointeur de souris normal
        QObject::connect(barreOutils->actionCrayon,     SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionLigne,      SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionRectVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionRectPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionElliVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionElliPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionTexte,      SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionAjoutPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionSupprPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionDeplacePnj, SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionEtatPnj,    SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));

        // Connexion de l'action avec l'affichage/masquage de l'image
        QObject::connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));

        // Mise a jour du pointeur de souris de l'image
        switch(G_outilCourant)
        {
                case BarreOutils::main :
                        imageFenetre->pointeurMain();
                        break;
                default :
                        imageFenetre->pointeurNormal();
                        break;
        }

        // Affichage de l'image
        imageFenetre->show();
}*/

/********************************************************************/
/* Appelle la fonction ouvrirPlan avec le parametre "masquer" egale */
/* a true                                                           */
/********************************************************************/
/*void MainWindow::ouvrirEtMasquerPlan()
{
        ouvrirPlan(true);
}*/

/********************************************************************/
/* Demande a l'utilisateur de selectionner un fichier image ou un   */
/* fichier plan, puis ouvre la carte. Le parametre "masquer" n'est  */
/* utilise que lorsque l'utilisateur ouvre une image                */
/********************************************************************/
/*void MainWindow::ouvrirPlan(bool masquer)
{

        QString fichier = QFileDialog::getOpenFileName(this, masquer?tr("Ouvrir et masquer un plan"):tr("Ouvrir un plan"), m_options->value("MapDirectory",QVariant(".")).toString(),
                tr("Plans (*.pla *.jpg *.jpeg *.png *.bmp)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Erreur de chargement"));
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        //! @todo : cross-platform issue ? should use QDir class.
        int dernierSlash = fichier.lastIndexOf("/");
        m_options->registerValue("MapDirectory",fichier.left(dernierSlash));

        // Suppression de l'extension du fichier pour obtenir le titre de la CarteFenetre
        int dernierPoint = fichier.lastIndexOf(".");
        QString titre = fichier.left(dernierPoint);
        titre = titre.right(titre.length()-dernierSlash-1);

        // 2 cas de figure possibles :
        // Le fichier est un plan, on l'ouvre
        if (fichier.right(4) == ".pla")
        {
                // Ouverture du fichier .pla
                QFile file(fichier);
                if (!file.open(QIODevice::ReadOnly))
                {
                        qWarning("Probleme a l'ouverture du fichier .pla (ouvrirPlan - MainWindow.cpp)");
                        return;
                }
                // Lecture de la carte et des PNJ
                lireCarteEtPnj(file, masquer, titre);
                // Fermeture du fichier
                file.close();
        }

        // Le fichier est une image : on cree une nouvelle carte
        else
        {
                // Chargement de l'image
                QImage image(fichier);
                // Verification du chargement de l'image
                if (image.isNull())
                {
                        msgBox.setText(tr("Format de fichier incorrect"));
                        msgBox.exec();
                        return;
                }
            // Creation de l'identifiant
            QString idCarte = QUuid::createUuid().toString();
                // On recupere la taille des PJ
            int tailleDesPj = 0;//G_listeUtilisateurs->taillePj();
                // Creation de la carte
            Map *carte = new Map(idCarte, &image, tailleDesPj, masquer,this);
                // Creation de la CarteFenetre
            MapFrame *carteFenetre = new MapFrame(carte, this);
                // Ajout de la carte au workspace
            ajouterCarte(carteFenetre, titre);

                // Envoie de la carte aux autres utilisateurs
                // On commence par compresser l'image (format jpeg) dans un tableau
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
                bool ok = image.save(&buffer, "jpeg", 60);
                if (!ok)
                        qWarning("Probleme de compression de l'image (ouvrirPlan - MainWindow.cpp)");

                // Taille des donnees
                quint32 tailleCorps =
                        // Taille du titre
                        sizeof(quint16) + titre.size()*sizeof(QChar) +
                        // Taille de l'identifiant
                        sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                        // Taille des PJ
                        sizeof(quint8) +
                        // Taille de l'info "masquer ou pas"
                        sizeof(quint8) +
                        // Taille de l'image
                        sizeof(quint32) + byteArray.size();

                // Buffer d'emission
                char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                // Creation de l'entete du message
                enteteMessage *uneEntete;
                uneEntete = (enteteMessage *) donnees;
                uneEntete->categorie = plan;
                uneEntete->action = chargerPlan;
                uneEntete->tailleDonnees = tailleCorps;

                // Creation du corps du message
                int p = sizeof(enteteMessage);
                // Ajout du titre
                quint16 tailleTitre = titre.size();
                memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
                p+=sizeof(quint16);
                memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
                p+=tailleTitre*sizeof(QChar);
                // Ajout de l'identifiant
                quint8 tailleId = idCarte.size();
                memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), idCarte.data(), tailleId*sizeof(QChar));
                p+=tailleId*sizeof(QChar);
                // Ajout de la taille des PJ
                memcpy(&(donnees[p]), &tailleDesPj, sizeof(quint8));
                p+=sizeof(quint8);
                // Ajout de l'info "plan masque au chargement?"
                quint8 masquerPlan = masquer;
                memcpy(&(donnees[p]), &masquerPlan, sizeof(quint8));
                p+=sizeof(quint8);
                // Ajout de l'image
                quint32 tailleImage = byteArray.size();
                memcpy(&(donnees[p]), &tailleImage, sizeof(quint32));
                p+=sizeof(quint32);
                memcpy(&(donnees[p]), byteArray.data(), tailleImage);
                p+=tailleImage;

                // Emission de l'image au serveur ou a l'ensemble des clients
                emettre(donnees, tailleCorps + sizeof(enteteMessage));
                // Liberation du buffer d'emission
                delete[] donnees;
        }
}*/

/********************************************************************/
/* Reconstitue la carte et les PNJ associes se trouvant dans le     */
/* fichier passe en parametre. Si le parametre masquer = true, on   */
/* masque l'ensemble du plan                                        */
/********************************************************************/
/*void MainWindow::lireCarteEtPnj(QFile &file, bool masquer, QString nomFichier)
{
        // Lecture de la carte

        // On recupere le titre
      /*  quint16 tailleTitre;
        file.read((char *)&tailleTitre, sizeof(quint16));
        QChar *tableauTitre = new QChar[tailleTitre];
        file.read((char *)tableauTitre, tailleTitre*sizeof(QChar));
        QString titre(tableauTitre, tailleTitre);
        // On recupere la taille des PJ
        quint8 taillePj;
        file.read((char *)&taillePj, sizeof(quint8));
        // On recupere le fond original
        quint32 tailleFondOriginal;
        file.read((char *)&tailleFondOriginal, sizeof(quint32));
        QByteArray baFondOriginal(tailleFondOriginal, 0);
        file.read(baFondOriginal.data(), tailleFondOriginal);
        // On recupere le fond
        quint32 tailleFond;
        file.read((char *)&tailleFond, sizeof(quint32));
        QByteArray baFond(tailleFond, 0);
        file.read((char *)baFond.data(), tailleFond);
        // On recupere la couche alpha
        quint32 tailleAlpha;
        file.read((char *)&tailleAlpha, sizeof(quint32));
        QByteArray baAlpha(tailleAlpha, 0);
        file.read((char *)baAlpha.data(), tailleAlpha);

        bool ok;
        // Creation de l'image de fond original
        QImage fondOriginal;
        ok = fondOriginal.loadFromData(baFondOriginal, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression du fond original (lireCarteEtPnj - Mainwindow.cpp)");
        // Creation de l'image de fond
        QImage fond;
        ok = fond.loadFromData(baFond, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression du fond (lireCarteEtPnj - Mainwindow.cpp)");
        // Creation de la couche alpha
        QImage alpha;
        ok = alpha.loadFromData(baAlpha, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression de la couche alpha (lireCarteEtPnj - Mainwindow.cpp)");
        // Si necessaire la couche alpha est remplie, de facon a masquer l'image
        if (masquer)
        {
                QPainter painterAlpha(&alpha);
                painterAlpha.fillRect(0, 0, alpha.width(), alpha.height(), G_couleurMasque);
        }

    // Creation de l'identifiant de la carte
    QString idCarte = QUuid::createUuid().toString();
        // Creation de la carte
    Carte *carte = new Carte(idCarte, &fondOriginal, &fond, &alpha, taillePj,this);
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte);
        // Ajout de la carte au workspace : si aucun nom de fichier n'est passe en parametre, il s'agit d'une lecture de
        // carte dans le cadre de l'ouverture d'un fichier scenario : on prend alors le titre associe a la carte. Sinon
        // il s'agit d'un fichier plan : on prend alors le nom du fichier
        if (nomFichier.isEmpty())
                ajouterCarte(carteFenetre, titre);
        else
                ajouterCarte(carteFenetre, nomFichier);

        // Liberation de la memoire allouee
        delete[] tableauTitre;

        // Lecture des PNJ

        // On recupere le nombre de personnages presents dans le message
        quint16 nbrPersonnages;
        file.read((char *)&nbrPersonnages, sizeof(quint16));

        for (int i=0; i<nbrPersonnages; i++)
        {
                // On recupere le nom
                quint16 tailleNom;
                file.read((char *)&tailleNom, sizeof(quint16));
                QChar *tableauNom = new QChar[tailleNom];
                file.read((char *)tableauNom, tailleNom*sizeof(QChar));
                QString nomPerso(tableauNom, tailleNom);
                // On recupere l'identifiant du perso
                quint8 tailleIdPerso;
                file.read((char *)&tailleIdPerso, sizeof(quint8));
                QChar *tableauIdPerso = new QChar[tailleIdPerso];
                file.read((char *)tableauIdPerso, tailleIdPerso*sizeof(QChar));
                QString idPerso(tableauIdPerso, tailleIdPerso);
                // On recupere le type du personnage
                quint8 type;
                file.read((char *)&type, sizeof(quint8));
                DessinPerso::typePersonnage typePerso = (DessinPerso::typePersonnage)type;
                // On recupere le numero de PNJ
                quint8 numeroPnj;
                file.read((char *)&numeroPnj, sizeof(quint8));
                // On recupere le diametre
                quint8 diametre;
                file.read((char *)&diametre, sizeof(quint8));
                // On recupere la couleur
                QRgb rgb;
                file.read((char *)&rgb, sizeof(QRgb));
                QColor couleurPerso(rgb);
                // On recupere le point central du perso
                qint16 centreX, centreY;
                file.read((char *)&centreX, sizeof(qint16));
                file.read((char *)&centreY, sizeof(qint16));
                QPoint centre(centreX, centreY);
                // On recupere l'orientation du perso
                qint16 orientationX, orientationY;
                file.read((char *)&orientationX, sizeof(qint16));
                file.read((char *)&orientationY, sizeof(qint16));
                QPoint orientation(orientationX, orientationY);
                // On recupere la couleur de l'etat de sante
                file.read((char *)&rgb, sizeof(QRgb));
                QColor couleurEtat(rgb);
                // On recupere le nom de l'etat
                quint16 tailleEtat;
                file.read((char *)&tailleEtat, sizeof(quint16));
                QChar *tableauEtat = new QChar[tailleEtat];
                file.read((char *)tableauEtat, tailleEtat*sizeof(QChar));
                QString nomEtat(tableauEtat, tailleEtat);
                // On recupere le numero de l'etat de sante
                quint16 numEtat;
                file.read((char *)&numEtat, sizeof(quint16));
                // On recupere l'information visible/cache
                quint8 visible;
                file.read((char *)&visible, sizeof(quint8));
                // On recupere l'information orientation affichee/masquee
                quint8 orientationAffichee;
                file.read((char *)&orientationAffichee, sizeof(quint8));

                // Creation du PNJ dans la carte
                DessinPerso *pnj = new DessinPerso(carte, idPerso, nomPerso, couleurPerso, diametre, centre, typePerso, numeroPnj);
                // S'il doit etre visible, on l'affiche (s'il s'agit d'un PNJ [c'est tjrs le cas] et que l'utilisateur est le MJ, alors on affiche automatiquement le perso)
                if (visible || (typePerso == DessinPerso::pnj && !G_joueur))
                        pnj->afficherPerso();
                // On m.a.j l'orientation
                pnj->nouvelleOrientation(orientation);
                // Affichage de l'orientation si besoin
                if (orientationAffichee)
                        pnj->afficheOuMasqueOrientation();
                // M.a.j de l'etat de sante du personnage
                DessinPerso::etatDeSante sante;
                sante.couleurEtat = couleurEtat;
                sante.nomEtat = nomEtat;
                pnj->nouvelEtatDeSante(sante, numEtat);
                // Affiche ou masque le PNJ selon qu'il se trouve sur une zone masquee ou pas
                carte->afficheOuMasquePnj(pnj);

                // Liberation de la memoire allouee
                delete[] tableauNom;
                delete[] tableauIdPerso;
                delete[] tableauEtat;
        }

        // Emission de la carte et des PNJ

        // On demande a la carte de s'emettre vers les autres utilisateurs
        carte->emettreCarte(carteFenetre->windowTitle());
        // On emet egalement l'ensemble des personnages presents sur la carte
        carte->emettreTousLesPersonnages();
}*/

/********************************************************************/
/* Demande a l'utilisateur de selectionner un fichier image puis    */
/* l'ouvre sous forme d'Image (image non modifiable)                */
/********************************************************************/
/*void MainWindow::ouvrirImage()
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir une image"), m_options->value(QString("ImageDirectory"),QVariant(".")).toString(),
                tr("Images (*.jpg *.jpeg *.png *.bmp)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Erreur de chargement"));
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // On met a jour le chemin vers les images
        int dernierSlash = fichier.lastIndexOf("/");
        m_options->registerValue(QString("ImageDirectory"),QVariant(fichier.left(dernierSlash)));

        // Chargement de l'image
        QImage *img = new QImage(fichier);
        // Verification du chargement de l'image
        if (img->isNull())
        {
                msgBox.setText(tr("Format de fichier incorrect"));
                msgBox.exec();
                delete img;
                return;
        }

        // Suppression de l'extension du fichier pour obtenir le titre de l'image
        int dernierPoint = fichier.lastIndexOf(".");
        QString titre = fichier.left(dernierPoint);
        titre = titre.right(titre.length()-dernierSlash-1);
        titre+= tr(" (Image)");

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

    // Creation de l'identifiant
    QString idImage = QUuid::createUuid().toString();

        // Creation de la fenetre image
        Image *imageFenetre = new Image(idImage, "", img, action);

        // Ajout de l'image a la liste (permet par la suite de parcourir l'ensemble des images)
        listeImage.append(imageFenetre);

        // Ajout de l'image au workspace
        workspace->addWidget(imageFenetre);

        // Mise a jour du titre de l'image
        imageFenetre->setWindowTitle(titre);

        // Connexion de l'action d'outil main avec la fonction de changement de pointeur de souris en main
        QObject::connect(barreOutils->actionMain,	    SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurMain()));

        // Connexion des actions de changement d'outil avec la fonction de changement de pointeur de souris normal
        QObject::connect(barreOutils->actionCrayon,     SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionLigne,      SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionRectVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionRectPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionElliVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionElliPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionTexte,      SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionAjoutPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionSupprPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionDeplacePnj, SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        QObject::connect(barreOutils->actionEtatPnj,    SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));

        // Connexion de l'action avec l'affichage/masquage de la fenetre
        QObject::connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));

        // Mise a jour du pointeur de souris de l'image
        switch(G_outilCourant)
        {
                case BarreOutils::main :
                        imageFenetre->pointeurMain();
                        break;
                default :
                        imageFenetre->pointeurNormal();
                        break;
        }

        // Affichage de l'image
        imageFenetre->show();

        // Envoie de l'image aux autres utilisateurs

        // On commence par compresser l'image (format jpeg) dans un tableau
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        bool ok = img->save(&buffer, "jpeg", 60);
        if (!ok)
                qWarning("Probleme de compression de l'image (ouvrirImage - MainWindow.cpp)");

        // Taille des donnees
       /* quint32 tailleCorps =
                // Taille du titre
                sizeof(quint16) + titre.size()*sizeof(QChar) +
                // Taille de l'identifiant de l'image
                sizeof(quint8) + idImage.size()*sizeof(QChar) +
                // Taille de l'identifiant du joueur
                //sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
                // Taille de l'image
                sizeof(quint32) + byteArray.size();

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = image;
        uneEntete->action = chargerImage;
        uneEntete->tailleDonnees = tailleCorps;

        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout du titre
        quint16 tailleTitre = titre.size();
        memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
        p+=tailleTitre*sizeof(QChar);
        // Ajout de l'identifiant de l'image
        quint8 tailleIdImage = idImage.size();
        memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
        p+=tailleIdImage*sizeof(QChar);
        // Ajout de l'identifiant du joueur
        quint8 tailleIdJoueur = G_idJoueurLocal.size();
        memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        // Ajout de l'image
        quint32 tailleImage = byteArray.size();
        memcpy(&(donnees[p]), &tailleImage, sizeof(quint32));
        p+=sizeof(quint32);
        memcpy(&(donnees[p]), byteArray.data(), tailleImage);
        p+=tailleImage;

        // Emission de l'image au serveur ou a l'ensemble des clients
       // emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;
}*/

/********************************************************************/
/* Ferme le plan ou l'image actuellement ouvert sur l'ordinateur    */
/* local et chez les autres utilisateurs                            */
/********************************************************************/
/*void MainWindow::fermerPlanOuImage()
{
        // On recupere la fenetre active (qui est forcement de type CarteFenetre ou Image, sans quoi l'action
        // ne serait pas dispo dans le menu Fichier)
        QMdiSubWindow *active = workspace->activeSubWindow();

        // Ne devrait jamais arriver
        if (!active)
        {
                qWarning("Action fermerPlan appelee alors qu'aucun widget n'est actif dans le workspace (fermerPlanOuImage - MainWindow.h)");
                return;
        }

        // On verifie pour le principe qu'il s'agit bien d'une CarteFenetre ou d'une Image
        if (active->objectName() != "CarteFenetre" && active->objectName() != "Image")
        {
                qWarning("Demande de fermeture d'un widget qui n'est ni une CarteFenetre, ni une Image (fermerPlanOuImage - MainWindow.h)");
                return;
        }

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(tr("Fermer"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
        // M.a.j du titre et du message
        if (active->objectName() == "CarteFenetre")
        {
                msgBox.setWindowTitle(tr("Fermer plan"));
                msgBox.setText(tr("Voulez-vous vraiment fermer ce plan?\nCelui-ci sera également fermé chez tous\nles autres utilisateurs"));
        }
        else
        {
                msgBox.setWindowTitle(tr("Fermer image"));
                msgBox.setText(tr("Voulez-vous vraiment fermer cette image?\nCelle-ci sera également fermée chez tous\nles autres utilisateurs"));
        }
        msgBox.exec();

        // Si l'utilisateur n'a pas clique sur "Fermer", on quitte
        if (msgBox.result() != QMessageBox::AcceptRole)
                return;

        // Emission de la demande de fermeture de la carte
        if (active->objectName() == "CarteFenetre")
        {
                // Recuperation de l'identifiant de la carte
                QString idCarte = ((MapFrame *)active)->carte()->identifiantCarte();

                // Taille des donnees
                quint32 tailleCorps =
                        // Taille de l'identifiant de la carte
                        sizeof(quint8) + idCarte.size()*sizeof(QChar);

                // Buffer d'emission
                char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                // Creation de l'entete du message
                enteteMessage *uneEntete;
                uneEntete = (enteteMessage *) donnees;
                uneEntete->categorie = plan;
                uneEntete->action = fermerPlan;
                uneEntete->tailleDonnees = tailleCorps;

                // Creation du corps du message
                int p = sizeof(enteteMessage);
                // Ajout de l'identifiant de la carte
                quint8 tailleIdCarte = idCarte.size();
                memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                p+=tailleIdCarte*sizeof(QChar);

                // Emission de la demande de fermeture de la carte au serveur ou a l'ensemble des clients
                //emettre(donnees, tailleCorps + sizeof(enteteMessage));
                // Liberation du buffer d'emission
                delete[] donnees;

                // Suppression de la CarteFenetre et de l'action associee sur l'ordinateur local
                ((MapFrame *)active)->~MapFrame();
        }

        // Emission de la demande de fermeture de l'image
        else
        {
                // Recuperation de l'identifiant de la carte
                QString idImage = ((Image *)active)->identifiantImage();

                // Taille des donnees
                quint32 tailleCorps =
                        // Taille de l'identifiant de la carte
                        sizeof(quint8) + idImage.size()*sizeof(QChar);

                // Buffer d'emission
                char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                // Creation de l'entete du message
                enteteMessage *uneEntete;
                uneEntete = (enteteMessage *) donnees;
                uneEntete->categorie = image;
                uneEntete->action = fermerImage;
                uneEntete->tailleDonnees = tailleCorps;

                // Creation du corps du message
                int p = sizeof(enteteMessage);
                // Ajout de l'identifiant de la carte
                quint8 tailleIdImage = idImage.size();
                memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
                p+=tailleIdImage*sizeof(QChar);

                // Emission de la demande de fermeture de l'image au serveur ou a l'ensemble des clients
                //emettre(donnees, tailleCorps + sizeof(enteteMessage));
                // Liberation du buffer d'emission
                delete[] donnees;

                // Suppression de l'Image et de l'action associee sur l'ordinateur local
                ((Image *)active)->~Image();
        }
}*/

/********************************************************************/
/* Met a jour la variable globale indiquant s'il faut afficher le   */
/* nom des nouveaux PJ                                              */
/********************************************************************/
/*void MainWindow::afficherNomsPj(bool afficher)
{
       // G_affichageNomPj = afficher;
}*/

/********************************************************************/
/* Met a jour la variable globale indiquant s'il faut afficher le   */
/* nom des nouveaux PNJ                                             */
/********************************************************************/
/*void MainWindow::afficherNomsPnj(bool afficher)
{
        //G_affichageNomPnj = afficher;
}*/

/********************************************************************/
/* Met a jour la variable globale indiquant s'il faut afficher le   */
/* numero des nouveaux PNJ                                          */
/********************************************************************/
/*void MainWindow::afficherNumerosPnj(bool afficher)
{
        //G_affichageNumeroPnj = afficher;
}*/

/********************************************************************/
/* Regarde quelle est la carte active et lui envoie une demande     */
/* de changement de taille pour les PJ qu'elle contient             */
/********************************************************************/
/*void MainWindow::changerTaillePj(int nouvelleTaille)
{
        // On recupere le widget actif dans le workspace
        QMdiSubWindow *widget = workspace->activeSubWindow();

        // S'il y a un widget actif on verifie qu'il s'agit d'un objet "CarteFenetre"
        if (widget)
                if (widget->objectName() == "CarteFenetre")
                {
                        // On effectue un changement de taille pour les personnages de la carte active
                        ((mapFrame *)widget)->carte()->changerTaillePjCarte(nouvelleTaille);
                }
}*/

/********************************************************************/
/* Emet aux clients ou au serveur une demande de changement de      */
/* taille des PJ                                                    */
/********************************************************************/
/*void MainWindow::emettreChangementTaillePj(int nouvelleTaille)
{
        // On recupere le widget actif dans le workspace
       * QMdiSubWindow *widget = workspace->activeSubWindow();

        // S'il y a un widget actif on verifie qu'il s'agit d'un objet "mapFrame"
        if (widget)
                if (widget->objectName() == "CarteFenetre")
                {
                        // Emission du changement de taille

                        // Recuperation de l'identifiant de la carte
                        QString idCarte = ((mapFrame *)widget)->carte()->identifiantCarte();

                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille de l'identifiant de la carte
                                sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                                // Taille de la nouvelle taille des PJ
                                sizeof(quint8);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = persoJoueur;
                        uneEntete->action = changerTaillePersoJoueur;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout de l'identifiant de la carte
                        quint8 tailleIdCarte = idCarte.size();
                        memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                        p+=tailleIdCarte*sizeof(QChar);
                        // Ajout de la nouvelle taille de PJ
                        quint8 taillePj = nouvelleTaille;
                        memcpy(&(donnees[p]), &taillePj, sizeof(quint8));
                        p+=sizeof(quint8);

                        // Emission de la demande de changement de taille des PJ au serveur ou a l'ensemble des clients
                        //emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;
                }
}*/

/********************************************************************/
/* Demande a la carte active de masquer/afficher le personnage dont */
/* l'identifiant est passe en parametre                             */
/********************************************************************/
/*void MainWindow::affichageDuPj(QString idPerso, bool afficher)
{
        // On recupere le widget actif dans le workspace
        QMdiSubWindow *widget = workspace->activeSubWindow();

        // S'il y a un widget actif on verifie qu'il s'agit d'un objet "CarteFenetre"
        if (widget)
                if (widget->objectName() == "CarteFenetre")
                {
                        // On demande a la carte d'afficher ou masquer le PJ
                        ((mapFrame *)widget)->carte()->affichageDuPj(idPerso, afficher);

                        // Emision de la demande d'affichage/masquage du PJ au serveur ou aux clients

                        // Recuperation de l'identifiant de la carte
                        QString idCarte = ((mapFrame *)widget)->carte()->identifiantCarte();

                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille de l'identifiant de la carte
                                sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                                // Taille de l'identifiant du PJ
                                sizeof(quint8) + idPerso.size()*sizeof(QChar) +
                                // Taille de l'info affichier/masquer
                                sizeof(quint8);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = persoJoueur;
                        uneEntete->action = afficherMasquerPersoJoueur;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout de l'identifiant de la carte
                        quint8 tailleIdCarte = idCarte.size();
                        memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                        p+=tailleIdCarte*sizeof(QChar);
                        // Ajout de l'identifiant du PJ
                        quint8 tailleIdPerso = idPerso.size();
                        memcpy(&(donnees[p]), &tailleIdPerso, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), idPerso.data(), tailleIdPerso*sizeof(QChar));
                        p+=tailleIdPerso*sizeof(QChar);
                        // Ajout de l'info afficher/masquer
                        quint8 afficherMasquer = afficher;
                        memcpy(&(donnees[p]), &afficherMasquer, sizeof(quint8));
                        p+=sizeof(quint8);

                        // Emission de la demande d'affichage/masquage de PJ au serveur ou a l'ensemble des clients
                        //emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;
                }
}*/

/********************************************************************/
/* Met a jour le selecteur de taille de la liste d'utilisateurs si  */
/* la carte dont l'identifiant est passe en parametre se trouve     */
/* etre la carte active                                             */
/********************************************************************/
/*void MainWindow::mettreAJourSelecteurTaille(QString idCarte, int taillePj)
{
        // On recupere la fenetre active
        QMdiSubWindow *active = workspace->activeSubWindow();

        // S'il y a une fenetre active
       /* if (active)
                // S'il s'agit d'une CarteFenetre
                if (active->objectName() == "CarteFenetre")
                        // S'il d'agit de la carte demandee
                        if (((CarteFenetre *)active)->carte()->identifiantCarte() == idCarte)
                                // Alors on met a jour le selecteur de taille de la liste d'utilisateurs
                                //G_listeUtilisateurs->majTaillePj(taillePj);*
}*/

/********************************************************************/
/* M.a.j l'espace de travail en fonction de la fenetre actuellement */
/* active                                                           */
/********************************************************************/
/*void MainWindow::mettreAJourEspaceTravail()
{
        // On recupere la fenetre active
        QMdiSubWindow *active = workspace->activeSubWindow();

        // S'il y a une fenetre active, on la passe a la fonction changementFenetreActive
        if (active)
                changementFenetreActive(active);
}*/

/********************************************************************/
/* Met a jour le selecteur de taille des PJ, ainsi que l'etat       */
/* d'affichage des PJ, si la fenetre activee dans le workspace est  */
/* de type CarteFenetre                                             */
/********************************************************************/
void MainWindow::changementFenetreActive(QMdiSubWindow *widget)
{
        // Si le widget existe on verifie qu'il s'agit d'un objet "CarteFenetre"
        /*if (widget)
        {
                if (widget->objectName() == "CarteFenetre")
                {


                        // Si l'utilisateur local est MJ on autorise la fermeture et la sauvegarde du plan
                        /*if (!G_joueur)
                        {
                                actionFermerPlan->setEnabled(true);
                                actionSauvegarderPlan->setEnabled(true);;
                        }
                        else
                                actionFermerPlan->setEnabled(false);
                        // On demande la taille des PJ dans la carte selectionnee
                       // int taille = ((mapFrame *)widget)->carte()->tailleDesPj();
                        // Puis on met a jour le selecteur de taille des PJ
                        //G_listeUtilisateurs->majTaillePj(taille);

                        // M.a.j de l'etat d'affichage de chaque PJ
                        //G_listeUtilisateurs->majAffichagePj(((CarteFenetre *)widget)->carte());
                }

                else if (widget->objectName() == "Tchat")
                {


                        // Si l'utilisateur local est MJ on interdit la fermeture et la sauvegarde des plans
                        /*if (!G_joueur)
                        {
                                actionFermerPlan->setEnabled(false);
                                actionSauvegarderPlan->setEnabled(false);;
                        }
                        else
                                actionFermerPlan->setEnabled(false);

                        // On decoche tous les PJ
                        //G_listeUtilisateurs->casesPjNonSelectionnables();

                        // Le joueur concerne ne doit plus clignoter
                        //G_listeUtilisateurs->nePlusFaireClignoter(((Tchat *)widget)->identifiant());
                }

                else if (widget->objectName() == "Image")
                {


                        // Si l'utilisateur local est MJ ou proprietaire de l'image on interdit la sauvegarde des plans et on autorise la fermeture de l'image
                        /*if (!G_joueur || ((Image *)widget)->proprietaireImage() )
                        {
                                actionFermerPlan->setEnabled(true);
                                actionSauvegarderPlan->setEnabled(false);;
                        }
                        else
                                actionFermerPlan->setEnabled(false);

                        // On decoche tous les PJ
                       // G_listeUtilisateurs->casesPjNonSelectionnables();
                }

                else if (widget->objectName() == "EditeurNotes")
                {

                        // On interdit la sauvegarde et la fermeture des plans
                        actionFermerPlan->setEnabled(false);
                        actionSauvegarderPlan->setEnabled(false);;

                        // On decoche tous les PJ
                        //G_listeUtilisateurs->casesPjNonSelectionnables();
                }

                else
                {


                        qWarning("Fenetre active autre que CarteFenetre, Tchat, Image ou EditeurNotes (changementFenetreActive - MainWindow.cpp)");
                        // On decoche tous les PJ
                        //G_listeUtilisateurs->casesPjNonSelectionnables();
                }
        }	// Fin du if(widget)

        // Aucun widget n'est actif
        else
        {

                // Si l'utilisateur local est MJ on interdit la fermeture et la sauvegarde des plans
                if (!G_joueur)
                {
                        actionFermerPlan->setEnabled(false);
                        actionSauvegarderPlan->setEnabled(false);;
                }
                else
                        actionFermerPlan->setEnabled(false);

                // On decoche tous les PJ
                //G_listeUtilisateurs->casesPjNonSelectionnables();
        }*/
}

/********************************************************************/
/* Demande une m.a.j des couleurs personnelles de la barre d'outils */
/********************************************************************/
/*void MainWindow::majCouleursPersonnelles()
{
        barreOutils->majCouleursPersonnelles();
}*/


void MainWindow::clickOnMapWizzard()
{

    MapWizzardDialog mapWizzard;
    //QTextStream out(stderr,QIODevice::WriteOnly);
    if(mapWizzard.exec())
    {


        Map* tempmap  = new Map();
        mapWizzard.setAllMap(tempmap);
        MapFrame* tmp = new MapFrame(tempmap);
        workspace->addWidget(tmp);
        tmp->show();

    }
}



/********************************************************************/
/* Envoie tous les plans deja ouvert au joueur dont l'identifiant   */
/* est passe en parametre (serveur uniquement)                      */
/********************************************************************/
/*void MainWindow::emettreTousLesPlans(QString idJoueur)
{
        // On recupere le numero de liaison correspondant a l'identifiant du joueur
        // (on soustrait 1 car le 1er utilisateur est toujours le serveur et qu'il
        // n'a pas de liaison associee)
        /*int numeroLiaison = G_listeUtilisateurs->numeroUtilisateur(idJoueur) - 1;

        // Taille de la liste des CarteFenetre
        int tailleListe = listeCarteFenetre.size();

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
        {
                // On demande a la carte contenue dans la CarteFenetre de s'emettre vers
                // l'utilisateur dont la liaison est passee en parametre
                listeCarteFenetre[i]->carte()->emettreCarte(listeCarteFenetre[i]->windowTitle(), numeroLiaison);
                // On emet egalement vers la liaison l'ensemble des personnages presents sur la carte
                listeCarteFenetre[i]->carte()->emettreTousLesPersonnages(numeroLiaison);
        }*
}*/

/********************************************************************/
/* Envoie toutes les images deja ouvertes au joueur dont            */
/* l'identifiant est passe en parametre (serveur uniquement)        */
/********************************************************************/
/*void MainWindow::emettreToutesLesImages(QString idJoueur)
{
        // On recupere le numero de liaison correspondant a l'identifiant du joueur
        // (on soustrait 1 car le 1er utilisateur est toujours le serveur et qu'il
        // n'a pas de liaison associee)
   //     int numeroLiaison = 0;//G_listeUtilisateurs->numeroUtilisateur(idJoueur) - 1;

        // Taille de la liste des Images
   //     int tailleListe = listeImage.size();

        // Parcours de la liste
    //    for (int i=0; i<tailleListe; i++)
                // On demande a l'Image de s'emettre vers l'utilisateur dont la liaison est passee en parametre
     //           listeImage[i]->emettreImage(listeImage[i]->windowTitle(), numeroLiaison);
}*/



/********************************************************************/
/* Affiche le tchat correspondant a l'utilisateur dont l'ID est     */
/* passe en parametre                                               */
/********************************************************************/
void MainWindow::afficherTchat(QString id)
{
        int i;
        bool trouve = false;
        int tailleListe = listeTchat.size();

        // Recherche du tchat
        for (i=0; i<tailleListe && !trouve; i++)
                if (listeTchat[i]->identifiant() == id)
                        trouve = true;

        // Ne devrait jamais arriver
        if (!trouve)
        {
                qWarning("Tchat introuvable et impossible a afficher (afficherTchat - MainWindow.cpp)");
                return;
        }

        // Affichage du tchat
        listeTchat[i-1]->show();
        // Mise a jour de l'action du sous-menu Tchats
        listeTchat[i-1]->majAction();
}

/********************************************************************/
/* Masque le tchat correspondant a l'utilisateur dont l'ID est      */
/* passe en parametre                                               */
/********************************************************************/
void MainWindow::masquerTchat(QString id)
{
        int i;
        bool trouve = false;
        int tailleListe = listeTchat.size();

        // Recherche du tchat
        for (i=0; i<tailleListe && !trouve; i++)
                if (listeTchat[i]->identifiant() == id)
                        trouve = true;

        // Ne devrait jamais arriver
        if (!trouve)
        {
                qWarning("Tchat introuvable et impossible a masquer (masquerTchat - MainWindow.cpp)");
                return;
        }

        // Masquage du tchat
        listeTchat[i-1]->hide();
        // Mise a jour de l'action du sous-menu Tchats
        listeTchat[i-1]->majAction();
}

/********************************************************************/
/* Ajoute un tchat                                                  */
/********************************************************************/
/*void MainWindow::ajouterTchat(QString idJoueur, QString nomJoueur)
{

        // Creation de l'action correspondante
        QAction *action = sousMenuTchat->addAction(tr("Tchat avec ") + nomJoueur);
        action->setCheckable(true);
        action->setChecked(false);
        // Creation du Tchat
        Tchat *tchat = new Tchat(idJoueur, action,NULL);
        // Connexion de l'action avec l'affichage/masquage de la fenetre
        QObject::connect(action, SIGNAL(triggered(bool)), tchat, SLOT(setVisible(bool)));
        // Ajout du tchat a la liste
        listeTchat.append(tchat);
        // Ajout du tchat au workspace
        workspace->addWidget(tchat);
        // Mise a jour du titre du tchat
        tchat->setWindowTitle(tr("Tchat avec ") + nomJoueur);
        // Masquage du tchat
        tchat->hide();
}*/

/********************************************************************/
/* Supprime le tchat associe au joueurs dont l'ID est passe en      */
/* parametre                                                        */
/********************************************************************/
/*void MainWindow::supprimerTchat(QString idJoueur)
{
        int i;
        bool trouve = false;
        int tailleListe = listeTchat.size();

        // Recherche du tchat
        for (i=0; i<tailleListe && !trouve; i++)
                if (listeTchat[i]->identifiant() == idJoueur)
                        trouve = true;

        // Ne devrait jamais arriver
        if (!trouve)
        {
                qWarning("Tchat introuvable et impossible a supprimer (supprimerTchat - MainWindow.cpp)");
                return;
        }

        // Suppression du Tchat
        listeTchat[i-1]->~Tchat();
        // Suppresion de l'element dans la liste
        listeTchat.removeAt(i-1);
}*/

/********************************************************************/
/* Renvoie le pointeur vers le Tchat dont l'identifiant est passe   */
/* en parametre, ou 0 si le Tchat n'est pas trouve                  */
/********************************************************************/
Tchat * MainWindow::trouverTchat(QString idJoueur)
{
        // Taille de la liste des Tchat
        int tailleListe = listeTchat.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if (listeTchat[i]->identifiant() == idJoueur)
                        ok = true;

        // Si le Tchat vient d'etre trouve on renvoie son pointeur
        if (ok)
                return listeTchat[i-1];
        // Sinon on renvoie 0
        else
                return 0;
}

/********************************************************************/
/* Renvoie true si la fenetre passee en parametre est la fenetre    */
/* active (fenetre de 1er plan) et qu'elle est affichee. Sinon      */
/* renvoie false                                                    */
/********************************************************************/
bool MainWindow::estLaFenetreActive(QWidget *widget)
{
        return widget == workspace->activeSubWindow() && widget->isVisible();
}

/********************************************************************/
/* Change la fenetre active du workspace devient celle passee en    */
/* parametre                                                        */
/********************************************************************/
/*void MainWindow::devientFenetreActive(QMdiSubWindow *widget)
{
        workspace->setActiveSubWindow(widget);
        changementFenetreActive(widget);
}*/

/********************************************************************/
/* Coche l'action Tchat commun du sous-menu Tchats                  */
/********************************************************************/
/*void MainWindow::cocherActionTchatCommun()
{
        actionTchatCommun->setChecked(true);
}*/

/********************************************************************/
/* Demande a l'utilisateur s'il desire reellement quitter           */
/* l'application, et si oui, ferme le programme. Si le parametre    */
/* perteConnexion = true alors la fermeture est due a une perte de  */
/* connexion avec le serveur                                        */
/********************************************************************/
void MainWindow::quitterApplication(bool perteConnexion)
{
        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        QAbstractButton *boutonSauvegarder 	= msgBox.addButton(tr("Save"), QMessageBox::YesRole);
        QAbstractButton *boutonQuitter 		= msgBox.addButton(tr("Quit"), QMessageBox::AcceptRole);
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // Creation du message
        QString message;

        // S'il s'agit d'une perte de connexion
        if (perteConnexion)
        {
                message = tr("La connexion avec le serveur a été perdue, ") + APPLICATION_NAME + tr(" va être fermé\n");
                // Icone de la fenetre
                msgBox.setIcon(QMessageBox::Critical);
                // M.a.j du titre et du message
                msgBox.setWindowTitle(tr("Perte de connexion"));
        }
        else
        {
                // Icone de la fenetre
                msgBox.setIcon(QMessageBox::Information);
                // Ajout d'un bouton
                QAbstractButton *boutonAnnuler = msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
                // M.a.j du titre et du message
                msgBox.setWindowTitle(tr("Quitter ") + APPLICATION_NAME);
        }

        // Si l'utilisateur est un joueur
        /*if (G_joueur)
                message += tr("Voulez-vous sauvegarder vos notes avant de quitter l'application?");

        // Si l'utilisateut est un MJ
        else
                message += tr("Voulez-vous sauvegarder le scénario avant de quitter l'application?");*/

        //M.a.j du message de la boite de dialogue
        msgBox.setText(message);
        // Ouverture de la boite d'alerte
        msgBox.exec();

        // Si l'utilisateur a clique sur "Quitter", on quitte l'application
        if (msgBox.clickedButton() == boutonQuitter)
        {
                // On sauvegarde les tchats
                //sauvegarderTousLesTchats();



                writeSettings();
                qApp->quit();
        }

        // Si l'utilisateur a clique sur "Sauvegarder", on applique l'action appropriee a la nature de l'utilisateur
        else if (msgBox.clickedButton() == boutonSauvegarder)
        {
                bool ok;
                // Si l'utilisateur est un joueur, on sauvegarde les notes
                /*if (G_joueur)
                        ok = sauvegarderNotes();
                // S'il est MJ, on sauvegarde le scenario
                else
                        ok = sauvegarderScenario();*/

                // Puis on quitte l'application si l'utilisateur a sauvegarde ou s'il s'agit d'une perte de connexion
                if (ok || perteConnexion)
                {
                        // On sauvegarde les tchats
                        //sauvegarderTousLesTchats();



                        writeSettings();
                        qApp->quit();
                }
        }
}

/********************************************************************/
/* L'utilisateur vient de cliquer sur l'icone de fermeture de la    */
/* fenetre principale, on lui demande de confirmer la fermture      */
/********************************************************************/
void MainWindow::closeEvent(QCloseEvent *event)
{
        // Arret de la procedure de fermeture
        event->ignore();


        // Demande de confirmation de la fermture a l'utilisateur
        quitterApplication();
}

/********************************************************************/
/* Renvoie le pointeur vers l'image dont l'identifiant est passe    */
/* en parametre, ou 0 si l'image n'est pas trouvee                  */
/********************************************************************/
Image *MainWindow::trouverImage(QString idImage)
{
        // Taille de la liste des Images
        int tailleListe = listeImage.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if (listeImage[i]->identifiantImage() == idImage)
                        ok = true;

        // Si l'Image vient d'etre trouve on renvoie son pointeur
        if (ok)
                return listeImage[i-1];
        // Sinon on renvoie 0
        else
                return 0;
}

/********************************************************************/
/* Enleve de la liste la carte dont l'ID est passe en parametre.    */
/* Renvoie true en cas de reussite, false sinon                     */
/********************************************************************/
/*bool MainWindow::enleverCarteDeLaListe(QString idCarte)
{
        // Taille de la liste des CarteFenetre
        int tailleListe = listeCarteFenetre.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if ( listeCarteFenetre[i]->carte()->identifiantCarte() == idCarte )
                        ok = true;

        // Si la carte vient d'etre trouvee on supprime l'element
        if (ok)
        {
                listeCarteFenetre.removeAt(i-1);
                return true;
        }
        // Sinon on renvoie false
        else
                return false;
}*/

/********************************************************************/
/* Enleve de la liste l'image dont l'ID est passe en parametre.     */
/* Renvoie true en cas de reussite, false sinon                     */
/********************************************************************/
bool MainWindow::enleverImageDeLaListe(QString idImage)
{
        // Taille de la liste des Images
        int tailleListe = listeImage.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if (listeImage[i]->identifiantImage() == idImage)
                        ok = true;

        // Si l'image vient d'etre trouvee on supprime l'element
        if (ok)
        {
                listeImage.removeAt(i-1);
                return true;
        }
        // Sinon on renvoie false
        else
                return false;
}

/********************************************************************/
/* Sauvegarde la plan actuellement ouvert ainsi que la position des */
/* PNJ et PJ                                                        */
/********************************************************************/
/*void MainWindow::sauvegarderPlan()
{
        // On recupere la fenetre active
        QMdiSubWindow *active = workspace->activeSubWindow();

        // On verifie pour le principe qu'il s'agit bien d'une CarteFenetre
        if (active->objectName() != "CarteFenetre")
        {
                qWarning("Demande de sauvegarde d'un plan qui n'est pas une CarteFenetre (sauvegarderPlan - MainWindow.h)");
                return;
        }

        // Ouverture du selecteur de fichiers
        QString fichier = QFileDialog::getSaveFileName(this, tr("Sauvegarder un plan"), m_options->value("MapDirectory",QVariant(".")).toString(), tr("Plans (*.pla)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        #ifdef MACOS
                // On rajoute l'extension
                fichier += ".pla";
        #endif

        // On met a jour le chemin vers les plans
        int dernierSlash = fichier.lastIndexOf("/");
        m_options->registerValue("MapDirectory",fichier.left(dernierSlash));

        // Creation du fichier
        QFile file(fichier);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (sauvegarderPlan - MainWindow.cpp)");
                return;
        }
        // On demande a la carte de se sauvegarder dans le fichier
        ((CarteFenetre *)active)->carte()->sauvegarderCarte(file);
        // Fermeture du fichier
        file.close();
}*/

/********************************************************************/
/* L'utilisateur local a demande a etre MJ lors de la connexion au  */
/* serveur mais un MJ etait deja connecte : l'utilisateur local     */
/* devient donc simple joueur. On met l'espace de travail a jour en */
/* consequence                                                      */
/********************************************************************/
/*void MainWindow::changementNatureUtilisateur()
{
        // M.a.j des menus du mainWindow
        autoriserOuInterdireActions();
        // M.a.j de la barre d'outils
        barreOutils->autoriserOuInterdireCouleurs();
        // M.a.j du lecteur audio (pour que le changement de taille se passe correctement, on enleve puis on remet le dockWidget)
        removeDockWidget(m_audioPlayer);
        m_audioPlayer->autoriserOuIntedireCommandes();
        addDockWidget(Qt::RightDockWidgetArea, m_audioPlayer);
        m_audioPlayer->show();
}*/

/********************************************************************/
/* Affiche ou masque l'editeur de notes. Le 2eme parametre doit     */
/* etre a true si la fonction est appelee a partir de l'objet       */
/* listeUtilisateurs ou de l'editeur de notes lui-meme : cela       */
/* permet de cocher/decocher l'action d'affichage/masquage          */
/********************************************************************/
void MainWindow::afficherEditeurNotes(bool afficher, bool cocherAction)
{
        // Affichage de l'editeur de notes
        editeurNotes->setVisible(afficher);

        // Si la fonction a pas ete appelee par la listeUtilisateurs ou par l'editeur lui-meme, on coche/decoche l'action associee
        if (cocherAction)
                actionEditeurNotes->setChecked(afficher);

        // Sinon on coche/decoche la case de l'editeur de notes dans la listeUtilisateurs
        else
        {
             /*   if (afficher)
                        G_listeUtilisateurs->cocherCaseTchat(G_idJoueurLocal);
                else
                        G_listeUtilisateurs->decocherCaseTchat(G_idJoueurLocal);*/
        }
}

/********************************************************************/
/* Demande a l'utilisateur de selectionner un fichier de notes a    */
/* ouvrir                                                           */
/********************************************************************/
/*void MainWindow::ouvrirNotes()
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir notes"), m_options->value("MinutesDirectory",QVariant(".")).toString(), tr("Documents HTML (*.htm *.html)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // On met a jour le chemin vers les notes
        int dernierSlash = fichier.lastIndexOf("/");
        m_options->registerValue("MinutesDirectory",fichier.left(dernierSlash));

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en lecture seule
        if (!file.open(QIODevice::ReadOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (ouvrirNotes - MainWindow.cpp)");
                return;
        }
        // On demande a l'editeur de notes de charger le fichier
        editeurNotes->ouvrirNotes(file);
        // Fermeture du fichier
        file.close();
}
*/
/********************************************************************/
/* Demande a l'utilisateur d'entrer un nom de fichier dans lequel   */
/* sauvegarder les notes                                            */
/********************************************************************/
/*bool MainWindow::sauvegarderNotes()
{
        // Ouverture du selecteur de fichiers
        QString fichier = QFileDialog::getSaveFileName(this, tr("Sauvegarder notes"), m_options->value("MinutesDirectory",QVariant(".")).toString(), tr("Documents HTML (*.htm *.html)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return false;


        if(!fichier.endsWith(".htm"))
                 fichier += ".htm";


       // On met a jour le chemin vers les notes
        int dernierSlash = fichier.lastIndexOf("/");
        m_options->registerValue("MinutesDirectory", fichier.left(dernierSlash));

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (sauvegarderNotes - MainWindow.cpp)");
                return false;
        }
        // On demande a l'editeur de notes de les sauvegarder dans le fichier
        editeurNotes->sauvegarderNotes(file);
        // Fermeture du fichier
        file.close();

        return true;
}*/

/********************************************************************/
/* Demande a l'utilisateur de selectionner un scenario a ouvrir     */
/********************************************************************/
/*void MainWindow::ouvrirScenario()
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir scénario"), m_options->value("ScriptDirectory",QVariant(".")).toString(), tr("Scénarios (*.sce)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // On met a jour le chemin vers les scenarii
        int dernierSlash = fichier.lastIndexOf("/");
        m_options->registerValue("scriptDirectory",fichier.left(dernierSlash));

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en lecture seule
        if (!file.open(QIODevice::ReadOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (ouvrirScenario - MainWindow.cpp)");
                return;
        }

        // On commence par lire le nbr de cartes a suivre
        quint16 nbrCartes;
        file.read((char *)&nbrCartes, sizeof(quint16));
        // On lit toutes les cartes presentes dans le fichier
        for (int i=0; i<nbrCartes; i++)
                lireCarteEtPnj(file);

        // On lit le nbr d'images a suivre
        quint16 nbrImages;
        file.read((char *)&nbrImages, sizeof(quint16));
        // On lit toutes les images presentes dans le fichier
        for (int i=0; i<nbrImages; i++)
                lireImage(file);

        // Enfin on lit les notes
        editeurNotes->ouvrirNotes(file);

        // Fermeture du fichier
        file.close();
}*/

/********************************************************************/
/* Demande a l'utilisateur de choisir un nom de fichier pour        */
/* sauvegarder le scenario en cours. Un scenario comprend toutes    */
/* les cartes, les images (qui deviennent proprietes du MJ), et les */
/* notes                                                            */
/********************************************************************/
/*bool MainWindow::sauvegarderScenario()
{
        // Ouverture du selecteur de fichiers
        QString filename = QFileDialog::getSaveFileName(this, tr("Sauvegarder scénario"), m_options->value("ScriptDirectory",QVariant(".")).toString(), tr("Scénarios (*.sce)"));


        if (filename.isNull())
                return false;

       if(!filename.endsWith(".sce"))
                filename += ".sce";


        // On met a jour le chemin vers les scenarii
        int dernierSlash = filename.lastIndexOf("/");
        m_options->registerValue("ScriptDirectory",filename.left(dernierSlash));

        // Creation du descripteur de fichier
        QFile file(filename);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (sauvegarderScenario - MainWindow.cpp)");
                return false;
        }

        // On commence par sauvegarder toutes les cartes
        sauvegarderTousLesPlans(file);
        // Puis toutes les images
        sauvegarderToutesLesImages(file);
        // Et enfin les notes
        editeurNotes->sauvegarderNotes(file);
        // Fermeture du fichier
        file.close();

        return true;
}*/

/********************************************************************/
/* Sauvegarde toutes les cartes dans le fichier file                */
/********************************************************************/
/*void MainWindow::sauvegarderTousLesPlans(QFile &file)
{
        // Taille de la liste des CarteFenetre
        quint16 tailleListe = listeCarteFenetre.size();
        // On ecrit le nbr de cartes a suivre
        file.write((char *)&tailleListe, sizeof(quint16));

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
                // On demande a la carte contenue dans la CarteFenetre de se sauvegarder dans le fichier
                listeCarteFenetre[i]->carte()->sauvegarderCarte(file, listeCarteFenetre[i]->windowTitle());
}*/

/********************************************************************/
/* Sauvegarde toutes les cartes dans le fichier file                */
/********************************************************************/
/*void MainWindow::sauvegarderToutesLesImages(QFile &file)
{
        // Taille de la liste des images
        quint16 tailleListe = listeImage.size();
        // On ecrit le nbr d'images a suivre
        file.write((char *)&tailleListe, sizeof(quint16));

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
                // On demande a l'image de se sauvegarder dans le fichier
                listeImage[i]->sauvegarderImage(file, listeImage[i]->windowTitle());
}*/

/********************************************************************/
/* Reconstitue l'image se trouvant dans le fichier passe en         */
/* parametre                                                        */
/********************************************************************/
/*void MainWindow::lireImage(QFile &file)
{
        // Lecture de l'image

        // On recupere le titre
        quint16 tailleTitre;
        file.read((char *)&tailleTitre, sizeof(quint16));
        QChar *tableauTitre = new QChar[tailleTitre];
        file.read((char *)tableauTitre, tailleTitre*sizeof(QChar));
        QString titre(tableauTitre, tailleTitre);
        // On recupere l'image
        quint32 tailleImage;
        file.read((char *)&tailleImage, sizeof(quint32));
        QByteArray baImage(tailleImage, 0);
        file.read(baImage.data(), tailleImage);

        bool ok;
        // Creation de l'image
        QImage img;
        ok = img.loadFromData(baImage, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression de l'image (lireImage - Mainwindow.cpp)");

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

    // Creation de l'identifiant
    QString idImage = QUuid::createUuid().toString();

        // Creation de la fenetre image
    Image *imageFenetre = new Image(idImage, "", &img, action);

        // Ajout de l'image a la liste (permet par la suite de parcourir l'ensemble des images)
        listeImage.append(imageFenetre);

        // Ajout de l'image au workspace
        workspace->addWidget(imageFenetre);

        // Mise a jour du titre de l'image
        imageFenetre->setWindowTitle(titre);

        // Connexion de l'action avec l'affichage/masquage de la fenetre
        QObject::connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));

        // Affichage de l'image
        imageFenetre->show();

        // Liberation de la memoire allouee
        delete [] tableauTitre;

        // Envoie de l'image aux autres utilisateurs

        // On commence par compresser l'image (format jpeg) dans un tableau
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        ok = img.save(&buffer, "jpeg", 60);
        if (!ok)
                qWarning("Probleme de compression de l'image (lireImage - MainWindow.cpp)");

        // Taille des donnees
        /*quint32 tailleCorps =
                // Taille du titre
                sizeof(quint16) + titre.size()*sizeof(QChar) +
                // Taille de l'identifiant de l'image
                sizeof(quint8) + idImage.size()*sizeof(QChar) +
                // Taille de l'identifiant du joueur
                sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
                // Taille de l'image
                sizeof(quint32) + byteArray.size();

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = image;
        uneEntete->action = chargerImage;
        uneEntete->tailleDonnees = tailleCorps;

        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout du titre
        quint16 tailleDuTitre = titre.size();
        memcpy(&(donnees[p]), &tailleDuTitre, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), titre.data(), tailleDuTitre*sizeof(QChar));
        p+=tailleDuTitre*sizeof(QChar);
        // Ajout de l'identifiant de l'image
        quint8 tailleIdImage = idImage.size();
        memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
        p+=tailleIdImage*sizeof(QChar);
        // Ajout de l'identifiant du joueur
        quint8 tailleIdJoueur = G_idJoueurLocal.size();
        memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        // Ajout de l'image
        quint32 tailleDeImage = byteArray.size();
        memcpy(&(donnees[p]), &tailleDeImage, sizeof(quint32));
        p+=sizeof(quint32);
        memcpy(&(donnees[p]), byteArray.data(), tailleDeImage);
        p+=tailleDeImage;

        // Emission de l'image au serveur ou a l'ensemble des clients
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;*
}*/

/********************************************************************/
/* Sauvegarde l'ensemble des tchats sous forme de fichiers HTML     */
/********************************************************************/
/*void MainWindow::sauvegarderTousLesTchats()
{
        // Taille de la liste des tchats
        int tailleListe = listeTchat.size();

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
        {
                // Creation du chemin complet du fichier
                QString chemin(m_options->value("TchatDirectory",QVariant(".")).toString() + "/" + listeTchat[i]->windowTitle() + ".htm");
                // Creation du descripteur de fichier
                QFile file(chemin);
                // Ouverture du fichier en ecriture seule
                if (!file.open(QIODevice::WriteOnly))
                {
                        qWarning("Probleme a l'ouverture du fichier (sauvegarderTousLesTchats - MainWindow.cpp)");
                        return;
                }
                // On demande au tchat de se sauvegarder dans le fichier
                listeTchat[i]->sauvegarderTchat(file);
                // Fermeture du fichier
                file.close();
        }
}*/




void MainWindow::about()
{
QMessageBox::about(this, tr("About Rolisteam"),
                 tr("<h1>Rolisteam v1.0</h1>"
"<p>Rolisteam makes easy the management of any role playing games. It allows players to communicate to each others, share maps or picture. Rolisteam also provides many features for: permission management, sharing background music and dices throw. Rolisteam is written in Qt4. Its dependencies are : Qt4 and Phonon.</p>"
"<p>Rolisteam may contain some files from the FMOD library. This point prevents commercial use of the software.</p> "
"<p>You may modify and redistribute the program under the terms of the GPL (version 2 or later).  A copy of the GPL is contained in the 'COPYING' file distributed with Rolisteam.  Rolisteam is copyrighted by its contributors.  See the 'COPYRIGHT' file for the complete list of contributors.  We provide no warranty for this program.</p>"
"<p><h3>URL:</h3>  <a href=\"http://code.google.com/p/rolisteam/\">http://code.google.com/p/rolisteam/</a></p> "
"<p><h3>BugTracker:</h3> <a href=\"http://code.google.com/p/rolisteam/issues/list\">http://code.google.com/p/rolisteam/issues/list</a></p> "
"<p><h3>Current developers :</h3> "
"<ul>"
"<li><a href=\"http://renaudguezennec.homelinux.org/accueil,3.html\">Renaud Guezennec</a></li>"
"</ul></p> "
"<p><h3>Retired developers :</h3>"
"<ul>"
"<li><a href=\"mailto:rolistik@free.fr\">Romain Campioni<a/> (rolistik)  </li>"
"</ul></p>"));
}

void MainWindow::readSettings()
{
        QSettings settings("RolisteamTeam", "Rolisteam");
        QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
        QSize size = settings.value("size", QSize(600, 400)).toSize();
        resize(size);
        move(pos);
        m_options->readSettings();

}
void MainWindow::writeSettings()
{
      QSettings settings("RolisteamTeam", "Rolisteam");
      settings.setValue("pos", pos());
      settings.setValue("size", size());
      m_options->writeSettings();

}


void MainWindow::aideEnLigne()
{

     QProcess *process = new QProcess;
     QStringList args;


#ifdef Q_WS_X11
     args << QLatin1String("-collectionFile")
             << QLatin1String("/usr/share/doc/rolisteam-doc/rolisteam.qhc");
            /* << QLatin1String("-register")
             << QLatin1String("/usr/share/doc/rolisteam-doc/rolisteam-manual.qch");*/
        process->start(QLatin1String("assistant"), args);

#elif defined Q_WS_WIN32
      args << QLatin1String("-collectionFile")
             << QLatin1String(qApp->applicationDirPath()+"/../resourcesdoc/rolisteam-doc/rolisteam.qhc");
      process->start(QLatin1String("assistant"), args);
#elif defined Q_WS_MAC
      QString a = QCoreApplication::applicationDirPath()+"/../Resources/doc/rolisteam.qhc";
     args << QLatin1String("-collectionFile")
             << QLatin1String(a.toLatin1());
      process->start(QLatin1String("/Developer/Applications/Qt/Assistant/Contents/MacOS/Assistant"), args);



#endif
            process->start(QLatin1String("assistant"), args);
             if (!process->waitForStarted())
                 return;


 }


