#include "Read_Env_Data.h"

ClassImp(Read_Env_Data);

//////////

Read_Env_Data::Read_Env_Data(const TString& a_data_file, vector<namespace_Env_Data::Data>& a_vec_data) : data_file(a_data_file), vec_data(a_vec_data)
{
  fin.open(data_file);
  if(fin.is_open()==kFALSE)
    {
      cerr << "Can not fine data file! " << data_file << endl;
      exit(1);
    }
  string buf(data_file);
  
  buf = buf.substr(buf.find_last_of('/')+1);
  
  stringstream ss;
  ss.str(buf);

  getline(ss, buf, '_');
  year = stoi(buf, nullptr);

  getline(ss, buf, '_');
  month = stoi(buf, nullptr);

  getline(ss, buf, '.');
  day = stoi(buf, nullptr);
  
  Read_Data();
}//Read_Env_Data::Read_Env_Data()

//////////

Read_Env_Data::~Read_Env_Data()
{
  fin.close();
}//Read_Env_Data::~Read_Env_Data()

//////////

void Read_Env_Data::Read_Data()
{
  while(!fin.eof())
    {
      string buf;
      getline(fin, buf);

      if(buf.compare("")==0) break;
      
      stringstream ss;
      ss.str(buf);

      getline(ss, buf, ':');
      int hour = stoi(buf, nullptr);

      getline(ss, buf, ':');
      int min = stoi(buf, nullptr);
      
      getline(ss, buf, ',');
      int sec = stoi(buf, nullptr);
            
      getline(ss, buf, ',');
      float temperature = stof(buf, nullptr) + 273.15;
            
      getline(ss, buf, ',');
      float pressure = stof(buf, nullptr);
            
      getline(ss, buf, ',');
      float relative_humidity = stof(buf, nullptr);

      namespace_Env_Data::Data data;
      data.year = year;
      data.month = month;
      data.day = day;
      data.hour = hour;
      data.min = min;
      data.sec = sec;
      data.temperature = temperature;
      data.pressure = pressure;
      data.relative_humidity = relative_humidity;

      vec_data.push_back(data);
    }
  
  return;
}//Read_Env_Data::Read_Data()

//////////
