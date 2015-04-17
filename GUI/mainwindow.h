#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <string.h>
#include <vector>
#include <stdint.h>

namespace Ui {
class MainWindow;
}

struct MOTE_STRUCT {
     uint16_t id;
     std::string device;
     std::string d_type;
     std::string sn;
     std::string directory;
     std::string port;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

	QString dirName;			 // dir name
    std::vector<MOTE_STRUCT> moteVector;

    void setDir(QString s);
    void  GetMoteDevice(std::vector< MOTE_STRUCT>&);
    void GetMotePorts(std::vector< MOTE_STRUCT >& mote_vector);

    void GetMsp430Device(std::vector< MOTE_STRUCT >& mote_vector);

    void makeOrInstallCommand(std::string  ,int mote_num);
    void MakeOrInstallMsp(std::string  ,int mote_num);
    void PrintMoteVector(std::vector< MOTE_STRUCT > moteVector );

    ~MainWindow();


private slots:

    void on_Make_button_clicked();

    void on_makeAllButton_clicked();

    void on_installAllButton_clicked();

    void on_installButton_clicked();

    void on_Open_Folder_Button_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
