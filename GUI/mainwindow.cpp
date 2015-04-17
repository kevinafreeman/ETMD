
#include <stdio.h>
#include <stdlib.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>

// Read ubs_list.txt for test
//#define UBS_LIST

using namespace std;

void MainWindow::GetMotePorts(std::vector< MOTE_STRUCT >& mote_vector)
{
    std::string str, searchStr, str_temp, str_f; // temp hold the id,device string
          MOTE_STRUCT  mote_ref, mote_ref1;

          size_t b_pos_n  , found;
        char cmda[] = "ls /dev/ttyACM* |wc -l";
         char buffera[255];
         FILE* pipe = popen(cmda, "r");
         if (!pipe)
             exit(1);

         while((fgets(buffera, 255, pipe)) != NULL){
              str += buffera; //todo
         }
         int x;
         x = std::atoi(str.c_str());
        //std::cout << "X: "<<x<<std::endl;
         pclose(pipe);
        char buffer[255];

         for(int i = 0; i < x; i++){
          std::ostringstream  convert;
            convert << i;
          std::string mycmd =  "udevadm info -q all -n /dev/ttyACM";
          mycmd.append(convert.str());
          mycmd.append(" --attribute-walk |grep \"MSP Debug Interface\"");
          char* cmd = (char*)mycmd.c_str();
          //std::cout << "CMD: " <<cmd<<std::endl;

         FILE* pipea = popen(cmd, "r");
         if (!pipea)
             exit(1);
           str.clear();
         while((fgets(buffer, 255, pipea)) != NULL){
                  str += buffer; //todo
         }
        // std::cout <<str<<std::endl;
         pclose(pipea);
         //str = result;
         // end exec non rf2500
         //std::cout<<"STR: "<<str<<std::endl;
         if( (found = str.find("MSP Debug Interface")) != std::string::npos)
         {
          //std::cout<<"in if statement for mote"<<std::endl;
            std::string myport = "/dev/ttyACM";
            myport.append(convert.str());
            mote_ref.id = moteVector.size() + 1;
            mote_ref.port = myport;
            mote_ref.d_type = "tilib";
            mote_vector.push_back( mote_ref);
         }
       }
//end non rf2500 mote

     char cmd1[] = "mspdebug --usb-list |grep eZ430";
     pipe = popen(cmd1, "r");
     if (!pipe)
         exit(1);

     while(!feof(pipe)) {
          if(fgets(buffer, 5, pipe) != NULL)
              str += buffer; //todo
     }
     pclose(pipe);
     //str = result;
     // end exec rf2500

     while(  (b_pos_n =  str.find('\n') ) != std::string::npos )
     {
           str_f = str.substr(0,b_pos_n+1);

           if(  (found = str_f.find("00") ) != std::string::npos ) {

                  // Find msp430 Port
                 searchStr = str_f.substr(found,7 );               //  Hold ID in string
                 //std::cout << "SearchStr: " << searchStr << std::endl;
                 mote_ref1.port = searchStr;
                 mote_ref1.d_type = "rf2500";

           }
           if(  (found = str_f.find("serial:") ) != std::string::npos ) {

                  // Find msp430 Port
                 //found = str_f.find("]");                               // Find ID's end
                 searchStr = str_f.substr(found+8,16 );               //  Hold ID in string
                 mote_ref1.sn = searchStr;
           }
           mote_ref1.id = moteVector.size() + 1;
           mote_vector.push_back( mote_ref1);
           str_temp =  str.substr(b_pos_n+1);
           str = str_temp ;
     }
//end rf2500 mote

}

void MainWindow::GetMsp430Device(std::vector< MOTE_STRUCT >& mote_vector){
    time_t start,end,total;
     string str_defaut_dir   = "/home/kevin/Repos/groupwork/CPSC8550/code/example";
    for(size_t i = 0; i<mote_vector.size(); i++){
        std::string str, searchStr, str_temp, fwupd8 ,flag; // temp hold the id,device string
      size_t b_pos_n, found;
        while((mote_vector[i].d_type != "rf2500" && mote_vector[i].d_type != "tilib")){
            if(i < mote_vector.size()-1){
                //std::cout<<i<<std::endl;
                i++;
            }else{
                break;
            }
        }
      if(mote_vector[i].d_type == "rf2500"){
        fwupd8 = "";
        flag = " -U ";
      }else{
        fwupd8 = " --allow-fw-update";
        flag = " -d ";
      }
      time(&start);
    std::string myCMD = ("sudo mspdebug " + mote_vector[i].d_type + flag + mote_vector[i].port + fwupd8 +" \"\" | grep Device:");
    const char * cmd = myCMD.c_str();
     //char cmd[] = myCMD;

     char buffer[255];
     FILE* pipe = popen(cmd, "r");
     if (!pipe)
         exit(1);

     while(!feof(pipe)) {
          if(fgets(buffer, 255, pipe) != NULL)
              str += buffer; //todo
     }
     pclose(pipe);
    // QString qstr = QString::fromStdString(str);   // convert string to QString
     //ui->textBrowserCmdOut->append(qstr);  // out text to textBrowser;
     // end exec non rf2500

     while(  (b_pos_n =  str.find('\n') ) != std::string::npos )
     {
           std::string  str_f = str.substr(0,b_pos_n+1);

           if(  (found = str_f.find("MSP") ) != std::string::npos ) {

                  // Find msp430 Port
                 searchStr = str_f.substr(found,12 );                     //  Hold ID in string
                 mote_vector[i].device = searchStr;
                 mote_vector[i].directory =str_defaut_dir;

           }

           str_temp =  str.substr(b_pos_n+1);
           str = str_temp ;
     }
     time(&end);
     total = end - start;
     std::cout<<"Time: "<<total<<std::endl;
    }

}

void MainWindow::makeOrInstallCommand(string s_cmd,int moteNum)
{
    string s_str = "cd ";
    s_str.append(moteVector [ moteNum ].directory) ;      // append directory
    s_str.append(" && make ");                                             // append  " &&  make "
    s_str.append( moteVector [ moteNum ].d_type );          // append mote type(telosb)

    if(s_cmd == "install") {  //add install string


        s_str.append( " install," );

        stringstream convert;
         convert <<moteVector[moteNum].id;
        s_str.append(convert.str());

        s_str.append(" bsl,");
        s_str.append( moteVector [ moteNum ].port);        // append mote devie
    }

    //s_str.append(" && cd - ");                                                  // append " && cd - " ,note must be " cd - "

    qDebug() << QString::fromStdString(s_str);

    // convert string to char
    char *c_cmd_make = new char[s_str.length()+1];
    strcpy( c_cmd_make,s_str.c_str() );

    // exec command
    char buffer[1024];
    std::string str,result;
    std::cout << "CMD: " << c_cmd_make <<std::endl;
    FILE* pipe = popen(c_cmd_make, "r");
    if (!pipe) {
         QMessageBox::information(this ,tr("Warning !"),tr("Make command exec error") ) ;
         return;
    }

    while(!feof(pipe)) {
         if(fgets(buffer, sizeof(buffer), pipe) != NULL)
               result += buffer; //todo
    }
   pclose(pipe);
   str = result;

   QString qstr = QString::fromStdString(str);   // convert string to QString
   ui->textBrowserCmdOut->append(qstr);
}

void MainWindow::MakeOrInstallMsp(std::string s_cmd,int moteNum){
    string s_str = "cd ";
    s_str.append(moteVector [ moteNum ].directory) ;      // append directory
    s_str.append(" && make DEVICE=");                                             // append  " &&  make "
    s_str.append(moteVector [ moteNum ].device) ;
    stringstream convert;

    s_str.append( " DEVICEID=" );
     convert <<moteVector[moteNum].id;// append " reinstall bsl,"
    s_str.append(convert.str());

    if(s_cmd == "install") {  //add install string


        s_str.append( " install DRIVER=" );
         s_str.append( moteVector [ moteNum ].d_type );


        s_str.append(" PORT=");
        s_str.append( moteVector [ moteNum ].port);        // append mote devie
    }

    //s_str.append(" && cd - ");                                                  // append " && cd - " ,note must be " cd - "

    qDebug() << QString::fromStdString(s_str);

    // convert string to char
    char *c_cmd_make = new char[s_str.length()+1];
    strcpy( c_cmd_make,s_str.c_str() );

    // exec command
    char buffer[1024];
    std::string str,result;
    std::cout << "CMD: " << c_cmd_make <<std::endl;
    FILE* pipe = popen(c_cmd_make, "r");
    if (!pipe) {
         QMessageBox::information(this ,tr("Warning !"),tr("Make command exec error") ) ;
         return;
    }

    while(!feof(pipe)) {
         if(fgets(buffer, sizeof(buffer), pipe) != NULL)
               result += buffer; //todo
    }
   pclose(pipe);
   str = result;

   QString qstr = QString::fromStdString(str);   // convert string to QString
   ui->textBrowserCmdOut->append(qstr);
}

void MainWindow::GetMoteDevice(vector< MOTE_STRUCT> &moteVector )
{
      string str, str_find ;   // temp hold the id,device string
      string str_defaut_dir   = "/opt/tinyos-main/apps/Blink";     // Default directory
       time_t start,end,total;
      MOTE_STRUCT  mote_ref;
      size_t b_pos_n, found  ;                                  // begin pos. size_t is an unsigned integral type

#ifdef UBS_LIST
     // read file for test function
     ifstream in_file;
     in_file.open("ubs_list.txt");
     stringstream strStream;
     strStream << in_file.rdbuf() ;           // read the file
     str = strStream.str();                        //str hold the file  
     // read file end file text in
#else
     //exec motelist command

     char cmd[] = "motelist";
     char buffer[255];
     FILE* pipe = popen(cmd, "r");
     if (!pipe) {
           QMessageBox::information(this ,tr("Warning !"),tr("Motelist command exec error !") ) ;
           exit(1);
     }
     while(!feof(pipe)) {
         if(fgets(buffer, 255, pipe) != NULL)
              str += buffer;
     }
     pclose(pipe);
     // end exec motelist command
#endif

     // print in textBrower
     QString qstr = QString::fromStdString(str);   // convert string to QString
     ui->textBrowserCmdOut->append(qstr);  // out text to textBrowser;

     //convert sting(str) to vector and put them to v_3str_mote
     // first line can't used
     b_pos_n = str.find('\n');
     string str_temp =  str.substr(b_pos_n+1);

     // second t line can't used
     b_pos_n = str_temp.find('\n');
     str =  str_temp.substr(b_pos_n+1);
    time(&start);
     while(  (b_pos_n =  str.find('\n') ) != string::npos )
     {
           std::string  str_f = str.substr(0,b_pos_n+1);

           if(  (found = str_f.find("Moteiv tmote sky") ) != string::npos ) {

                  // Find SN
                 found = str_f.find(" ");                               // Find SN's end
                 str_find = str_f.substr(0,found );               //  Hold ID in string
                 int founda;

                 founda = str_f.find("ttyUSB");
                std::string convert1;
                convert1 = str_f.substr(founda+12, str_f.length());
                mote_ref.device = convert1;
                 mote_ref.sn = str_find;

                 // Find PORT
                 found = str_f.find("/dev");
                 string str1 =  str_f.substr(found);             //  Get be found string
                 found = str1.find(" ");                                //  Find port's end
                 str_find = str1.substr(0,found);;                //  Hold device in string

                 mote_ref.port  = str_find;                      // Hold device
                 mote_ref.d_type  = "tmote";
                 mote_ref.directory = str_defaut_dir;
                 mote_ref.id = moteVector.size() +1;
                 moteVector.push_back( mote_ref);
           }
           else  if(  (  ( found = str_f.find("Moteiv Telos") )  != string::npos )  ||
                          (  ( found = str_f.find("XBOW Crossbow Telos") )!= string::npos )  )
          {
                    int founda;

                    founda = str_f.find("ttyUSB");
                   std::string convert1;
                   convert1 = str_f.substr(founda+12, str_f.length());
                   mote_ref.device = convert1;

                // Find SN
                 found = str_f.find(" ");                               // Find SN's end
                 str_find = str_f.substr(0,found );               //  Hold ID in string

                 mote_ref.sn = str_find;

                 // Find PORT
                 found = str_f.find("/dev");
                 string str1 =  str_f.substr(found);            //  Get be found string
                 found = str1.find(" ");                               //  Find port's end
                 str_find = str1.substr(0,found);;               //  Hold device in string

                 mote_ref.port = str_find;

                 // Find Rev
                 size_t found1;
                 if( (found1= str_f.find("Rev") ) != string::npos  ){
                        string str1 =  str_f.substr(found1);
                        str_find = str1.substr(4,1);;                      //  Get Rev
                        transform(str_find.begin(),str_find.end(), str_find.begin(), ::tolower);
                        string str_type ;
                        str_type.append("telos");
                        str_type.append(str_find);

                        mote_ref.d_type  = str_type;                   // Hold Rev
                        mote_ref.directory = str_defaut_dir;
                        mote_ref.id = moteVector.size() +1;
                        moteVector.push_back( mote_ref);
                  }
           }
           str_temp =  str.substr(b_pos_n+1);
           str = str_temp ;
     }
     time(&end);
     total = end - start;
     std::cout<<"TimeTmote: "<<total<<std::endl;
}
void PrintMoteVector(vector< MOTE_STRUCT> moteVector ){

      for (size_t i= 0;i<moteVector.size();i++) {
          cout << moteVector[i].id<< "\t";
          cout << moteVector[i].device<< "\t";
          cout << moteVector[i].d_type<< "\t";
          cout << moteVector[i].directory<< "\t";
          cout << moteVector[i].sn<< endl;
      }

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setDir("/opt/tinyos-main/apps/Blink");    // default dir
    QMessageBox::information(this ,tr("Please Wait"),tr("Please Wait while the devices are populated. A new page will appear after all devices are found and added to the page.") ) ;

    GetMoteDevice(moteVector);              // Run "motelist" command to Get mote parameter store in vector vMotePara

    GetMotePorts(moteVector);
    GetMsp430Device(moteVector);

    int v_size = moteVector.size();

    // Test whether mote
    if(v_size == 0) {  //no mote
        QMessageBox::information(this ,tr("Mote Warning !"),tr(" Can not find mote !") ) ;
        exit(1);
        //return  ;
    }
    else {
        // Set column and row count
        const int column_count = 5;        // Total column_count 4
        const int column_0_width = 180;
        const int column_1_width = 25;
        const int column_2_width = 120;
        const int column_3_width = 232;    // the 3 column width is 240
        const int column_4_width = 237;

        ui->tableWidget->setColumnCount( column_count );
        ui->tableWidget->setRowCount( v_size );
        ui->tableWidget->setColumnWidth(0,column_0_width);
        ui->tableWidget->setColumnWidth(1,column_1_width);
        ui->tableWidget->setColumnWidth(2,column_2_width);
        ui->tableWidget->setColumnWidth(3,column_3_width);    //Sets the width of the given column to be width.
        ui->tableWidget->setColumnWidth(4,column_4_width);
        // Fill VerticalHeaderLabels from 0 to v_size-1
        QStringList list;
        for(int i=0;i<v_size ;i++) {
            // convert number to string
            string str_result;                   //string which will contain the result
            stringstream convert;          // stringstream used for the conversion
            convert << (i);                        //add the value of Number to the characters in the stream
            str_result= convert.str();       //set Result to the content of the stream
            list <<  QString::fromStdString(str_result ) ;  // convert string to QString
        }
        //ui->tableWidget->setHorizontalHeaderLabels(list);
        ui->tableWidget->setVerticalHeaderLabels(list);

        // fill into table


        for(int i=0;i<v_size ;i++)
        {
                // fill SN
                //mote= vMotePara [ i] ;
                QTableWidgetItem *newItem0 = new QTableWidgetItem( QString::fromStdString( moteVector [ i ].sn) );  // convert string to QString
                ui->tableWidget->setItem(i,0,newItem0);

                //fill SN from 1 to v_size
                // convert number to string
                stringstream convert;                               // stringstream used for the conversion
                convert << moteVector[i].id;                                        //add the value of Number to the characters in the stream
                QTableWidgetItem *newItem1 = new QTableWidgetItem( QString::fromStdString(convert.str() ) );  // convert string to QString
                ui->tableWidget->setItem(i,1,newItem1);    //

                // fill port
                //mote= vMotePara [ i] ;
                QTableWidgetItem *newItem2 = new QTableWidgetItem( QString::fromStdString( moteVector [ i ].port) );  // convert string to QString
                ui->tableWidget->setItem(i,2,newItem2);    //

                // fill dir
                //mote= vMotePara [ i] ;
                QTableWidgetItem *newItem3 = new QTableWidgetItem( QString::fromStdString( moteVector [ i ].directory) );  // convert string to QString
                ui->tableWidget->setItem(i,3,newItem3);

                QTableWidgetItem *newItem4 = new QTableWidgetItem(QString::fromStdString(moteVector[i].device));  // convert string to QString
                ui->tableWidget->setItem(i,4,newItem4);


        }
        ui->tableWidget->verticalHeader()->setVisible( true );
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDir(QString s){
     dirName = s;
     ui->listWidgetDir->insertItem(0,dirName);
}

void MainWindow::on_Make_button_clicked()
{

    QList <QTableWidgetSelectionRange> list= ui->tableWidget->selectedRanges();    // Get multi row cumm

    if(list.isEmpty()) {
        QMessageBox::information(this ,tr("Make Warning !"),tr("Please select item") ) ;
        return;
    }
    else {     
        int cnt = list.count();

        for(int j=0; j<cnt ; j++)      // mult list
        {
             int lenght = list[j].bottomRow() - list[j].topRow();
             int moteNum = list[j].topRow();   // which mote begin
             std::cout<<moteNum<< std::endl;
             for(int i=0; i< lenght+1 ;i++)
             {
                 if(moteVector[moteNum].d_type == "rf2500" || moteVector[moteNum].d_type== "tilib"){
                     MakeOrInstallMsp("make", moteNum+i);
                 }else{
                  makeOrInstallCommand("make", moteNum+i ); // exec make command
                 }
             }
        }     
    }
}

void MainWindow::on_makeAllButton_clicked()
{
     int cnt = moteVector.size();

     for(int i=0; i<cnt ; i++)      // all mote
     {
         if(moteVector[i].d_type == "rf2500" || moteVector[i].d_type== "tilib"){
             MakeOrInstallMsp("make", i);
         }else{
          makeOrInstallCommand("make", i );     // exec make command
         }
     }
}

void MainWindow::on_installButton_clicked()
{
    QList <QTableWidgetSelectionRange> list= ui->tableWidget->selectedRanges();    // Get multi row cumm

    if(list.isEmpty()) {
        QMessageBox::information(this ,tr("Install Warning !"),tr("Please select item") ) ;
        return;
    }
    else {
        //MOTE_STRUCT  mote ;
        int cnt = list.count();

        for(int j=0; j<cnt ; j++)      // mult list
        {
             int lenght = list[j].bottomRow() - list[j].topRow();
             int moteNum = list[j].topRow();                                              // which mote begin
             for(int i=0; i< lenght+1 ;i++)
             {

                 if(moteVector[moteNum].d_type == "rf2500" || moteVector[moteNum].d_type== "tilib"){
                     MakeOrInstallMsp("install", moteNum+i);
                 }else{
                 makeOrInstallCommand("install", moteNum+i ); // exec install command
                 }
             }
        }
    }
}

void MainWindow::on_installAllButton_clicked()
{
    QMessageBox::information(this ,tr("Installing to All Devices"),tr("Installing to All deviecs\nAnother message will appear when all devices have been programmed.") );

    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    QCoreApplication::processEvents();
    //MOTE_STRUCT  mote ;
    int cnt = moteVector.size();

    for(int i=0; i<cnt ; i++)                                     // all mote
    {
        if(moteVector[i].d_type == "rf2500" || moteVector[i].d_type== "tilib"){
            MakeOrInstallMsp("install", i);
        }else{
         makeOrInstallCommand("install", i );      // exec install all command
        }
    }
    QMessageBox::information(this ,tr("Installing to All Devices: COMPLETE"),tr("Installing to all devices complete"));
}

void MainWindow::on_Open_Folder_Button_clicked()
{
    dirName = QFileDialog::getExistingDirectory(this,tr("Open Dir"),dirName,QFileDialog::ShowDirsOnly);

    ui->listWidgetDir->insertItem(0,dirName);

     ui->textBrowserCmdOut->append(dirName);  // out text to textBrowser;

     QTableWidgetItem *newItem = new QTableWidgetItem(dirName);

      int current_row = ui->tableWidget->currentRow();   //Returns the row of the current item.
      if(current_row != -1) {
             ui->tableWidget->setItem(current_row,3,newItem);    // Get the sigle row
             //MOTE_STRUCT  mote ;
             moteVector [ current_row].directory = dirName.toStdString();
      }
}
