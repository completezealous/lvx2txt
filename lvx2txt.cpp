#include <iostream>
#include <fstream>

#pragma pack(push, 1)  // 确保结构体按照字节对齐方式进行打包

// Public Header结构体
struct PublicHeader {
    char filesignature[16];
    uint8_t field1;
    uint8_t field2;
    uint8_t field3;
    uint8_t field4;
    char code[4];
    // 添加其他字段...
};
// Private Header结构体
struct PrivateHeader {
    unsigned int frame_duration;
    unsigned char device;
    // 添加其他字段...
};

// Device Info结构体
struct DeviceInfo {
    char Lidar_sn[16];
    char hub_sn[16];
    unsigned char device_ind;
    unsigned char device_type;
    unsigned char extrinsic_enable;
    float roll;
    float pitch;    
    float yaw;
    float x;    
    float y;
    float z;
    // 添加其他字段...
};

// Frame Header结构体
struct FrameHeader {
    long long cur_offset;
    long long next_offset;
    long long frameindex;
    // 添加其他字段...
};
// Package结构体
struct Package {
    unsigned char device_ind;
    uint8_t version;
    uint8_t slot_id;
    uint8_t Lidar_id;
    uint8_t rsd;
    uint32_t status_code;
    uint8_t timestamp_type;
    uint8_t data_type;
    uint64_t timestamp;
};
struct Pointdata{
    int32_t x;
    int32_t y;
    int32_t z;
    uint8_t intensity;
    uint8_t tag;   
};
struct Datatype_6{
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float acc_x;
    float acc_y;
    float acc_z;
};
#pragma pack(pop)  // 恢复默认的字节对齐方式

int main() {
    std::ifstream file("C:/Users/lk/Desktop/2.lvx", std::ios::binary);  // 打开二进制文件
    if (!file) {
        std::cout << "无法打开文件." << std::endl;
        return 1;
    }
    std::ofstream output_file("output.txt");
    
    // 读取Public Header
    PublicHeader publicHeader;
    file.read(reinterpret_cast<char*>(&publicHeader), sizeof(PublicHeader));
    
    // 读取Private Header
    PrivateHeader privateHeader;
    file.read(reinterpret_cast<char*>(&privateHeader), sizeof(PrivateHeader));
    // 读取DeviceInfo
    DeviceInfo deviceInfo;
    file.read(reinterpret_cast<char*>(&deviceInfo), sizeof(DeviceInfo));
    
    //output_file << "x,y,z,intensity,tag" << '\n';
    // 读取Point Data
    while (file.peek() != EOF)  {
        long long bytes=24;//frameheader字节24
        // 读取Frame Header
        FrameHeader frameHeader;
        Datatype_6 datatype_6;
        file.read(reinterpret_cast<char*>(&frameHeader), sizeof(FrameHeader));
        //output_file <<"frameHeader.cur_offset:" <<frameHeader.cur_offset << '\n';
        //output_file <<"frameHeader.next_offset:" << frameHeader.next_offset << '\n';
        //output_file <<"frameHeader.frameindex:" << frameHeader.frameindex << '\n';

        // 计算每个Frame的字节数
        uint32_t frameSize =frameHeader.next_offset-frameHeader.cur_offset;
        //output_file <<"frameSize:" << frameSize << '\n';
        
        while(bytes<frameSize){
            Package package;
            file.read(reinterpret_cast<char*>(&package), sizeof(Package));
            bytes+=19;
            //output_file << "datatype:" << static_cast<int>(package.data_type) << '\n';

            Pointdata pointdata;
            //file.read(reinterpret_cast<char*>(&pointdata), sizeof(Pointdata));

            if(package.data_type==6){
                file.read(reinterpret_cast<char*>(&datatype_6), sizeof(Datatype_6));
                bytes+=24;
            }
            else if(package.data_type==2){
              for(int i=0;i<96;i++){
                file.read(reinterpret_cast<char*>(&pointdata), sizeof(Pointdata));
                output_file << double((pointdata.x)/1000.3f)<< ','<< double((pointdata.y)/1000.3f) << ','<< double((pointdata.z)/1000.3f) << ','<< static_cast<float>(pointdata.intensity) << '\n';   
                }
                bytes+=96*14;
            }

        }
   }
    file.close();
    output_file.close();
    return 0;
 }
