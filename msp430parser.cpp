#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdint.h>

struct MOTE_STRUCT {
     uint16_t id;
     std::string device;
     std::string d_type;
     std::string sn;
     std::string directory;
     std::string port;
};


void GetMotePorts(std::vector< MOTE_STRUCT >& mote_vector)
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
    std::cout << "X: "<<x<<std::endl;
     pclose(pipe);
    char buffer[255];

     for(int i = 0; i < x; i++){
      std::ostringstream  convert;
        convert << i;
      std::string mycmd =  "udevadm info -q all -n /dev/ttyACM";
      mycmd.append(convert.str());
      mycmd.append(" --attribute-walk |grep \"MSP Debug Interface\"");
      char* cmd = (char*)mycmd.c_str();
      std::cout << "CMD: " <<cmd<<std::endl;
     
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
     std::cout<<"STR: "<<str<<std::endl;
     if( found = str.find("MSP Debug Interface") != std::string::npos)
     {
      std::cout<<"in if statement for mote"<<std::endl;
        std::string myport = "/dev/ttyACM";
         myport.append(convert.str());
        mote_ref.port = myport;
        mote_ref.d_type = "tilib";
        mote_vector.push_back( mote_ref);
     }
   }
//end non rf2500 mote

     char cmd1[] = "mspdebug --usb-list |grep eZ430";
     pipe = popen(cmd1, "r");
     str.clear();
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
		   mote_vector.push_back( mote_ref1);
           str_temp =  str.substr(b_pos_n+1);
           str = str_temp ;
     }
//end rf2500 mote

}

void GetMoteDevice(std::vector< MOTE_STRUCT >& mote_vector){
	for(int i = 0; i<mote_vector.size(); i++){
		std::string str, searchStr, str_temp, fwupd8,flag; // temp hold the id,device string
      MOTE_STRUCT  mote_ref;

      size_t b_pos_n, found;

      if(mote_vector[i].d_type == "rf2500"){
      	fwupd8 = "";
        flag = " -U ";
      }else{
      	fwupd8 = " --allow-fw-update";
        flag = " -d ";
      }
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
     //str = result;
     // end exec non rf2500

     while(  (b_pos_n =  str.find('\n') ) != std::string::npos )
     {
           std::string  str_f = str.substr(0,b_pos_n+1);

           if(  (found = str_f.find("MSP") ) != std::string::npos ) {

                  // Find msp430 Port
                 //found = str_f.find(" ");                               // Find ID's end
                 searchStr = str_f.substr(found,12 );               //  Hold ID in string
                 mote_vector[i].device = searchStr;
                 
           }

           str_temp =  str.substr(b_pos_n+1);
           str = str_temp ;
     }
	}

}

int main(){
	std::vector<MOTE_STRUCT> myMotes;
	GetMotePorts(myMotes);
	GetMoteDevice(myMotes);
	for(int x = 0; x< myMotes.size(); x++){
		std::cout << x <<": " << myMotes[x].port << " \t " << myMotes[x].device << " \t " << myMotes[x].sn << " \t " <<myMotes[x].d_type << std::endl;
	}
}