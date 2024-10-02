#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cinttypes>
#include <cstring>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <type_traits>
#include <iostream>
#include <algorithm>
#include <memory>

#include "xmem.h"

typedef uint8_t datatype_t;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define QUOTE(...) #__VA_ARGS__


typedef struct{
    uint32_t width;
    uint32_t len;
    bool dynamic_array;
    datatype_t datatype;
    std::queue<uint8_t> content;
}signal_info_t;

//#define DEBUG 1

class Ctgload{
public:
    Ctgload(){
        test_count = 0;
    }

    bool open(const char *filepath)
    {
        bool rc = true;
        FILE* fi = NULL;


        const char *search_path[] = {"",
                                    "../../../",
                                    "../../../capture_data/"
        };

        uint16_t qnum = 0;
        std::string signame;
        signal_info_t sig_info;

        bool file_exist = false;
        std::string exist_filepath;
        for (unsigned int i=0; i<ARRAY_SIZE(search_path); i++){
            exist_filepath.assign(search_path[i]);
            exist_filepath.append(filepath);
            if (access(exist_filepath.c_str(), F_OK) == 0) {
                //std::cout << exist_filepath << " exists.\n";
                file_exist = true;
                break;
            }
        }

        if (!file_exist) {
            std::cout << filepath << " does not exist in dedicated directories .\n";
            rc = false;
            goto EXIT;
        }

        fi = fopen(exist_filepath.c_str(), "rb");
        if (fi == NULL){
            std::cout << filepath << " could not be opened.\n";
            rc = false;
            goto EXIT;
        }

        if (fread(&qnum, 1, sizeof(qnum), fi) != sizeof(qnum)) {
            fclose(fi);
            rc = false;
            goto EXIT;
        }

        if (fread(&test_count, 1, sizeof(test_count), fi) != sizeof(test_count)) {
            fclose(fi);
            rc = false;
            goto EXIT;
        }

        for (int i = 0; i < qnum; i++) {
            char tmpName[65];
            if (fread(tmpName, 1, 64, fi) != 64) {
                qnum = 0;
                rc = false;
                goto EXIT;
            }
            tmpName[64] = '\0';
            signame = trim_string(std::string(tmpName));

            if (fread(&sig_info.datatype, sizeof(sig_info.datatype), 1, fi) != 1) {
                qnum = 0;
                rc = false;
                goto EXIT;
            }

            if (fread(&sig_info.width, sizeof(sig_info.width), 1, fi) != 1) {
                qnum = 0;
                rc = false;
                goto EXIT;
            }
            if (fread(&sig_info.len, sizeof(sig_info.len), 1, fi) != 1) {
                qnum = 0;
                rc = false;
                goto EXIT;
            }

            load_order.push_back(signame);
            sinfo[signame] = sig_info;
            std::cout << "tgload:" << signame << ",width:" << sig_info.width << ",len:" << sig_info.len << '\n';
        }

        for (int i = 0; i < qnum; i++) {
            const std::string &str = load_order[i];
            size_t sigwidth = sinfo[str].width;
            size_t qlen = sinfo[str].len;
            std::unique_ptr<char> byte (new char[sigwidth]);
            for (size_t j=0; j<qlen; j++) {
                if (fread(byte.get(), sigwidth, 1, fi) != 1) {
                    qnum = 0;
                    rc = false;
                    goto EXIT;
                }
                for (size_t j=0; j<sigwidth; j++){
                    sinfo[str].content.push(byte.get()[j]);
                }
            }
        }
        
    EXIT:
        if(fi != NULL) {
            fclose(fi);
        }
        if (!rc) {
            //exit immediately with error code
            exit(-1);
        }
        return rc;
    }

    size_t get_count(){
        return test_count;
    }


    void pop(std::istringstream &paralist_str) {
    }

    // Variadic template function to handle multiple variables
    template <typename T, typename... Args>
    typename std::enable_if<!std::is_class<T>::value, void>::type
    pop(std::istringstream &paralist_str, T &data, Args&... args){
        std::string varname;
        std::getline (paralist_str, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << "p:" << (int)get_datatype(data) << ' ' << varname << ' ' << sinfo[varname].width << '\n';
#endif
        pop_data(varname, &data);

        pop(paralist_str, args...); // Recursively dump the remaining variables
    }

    template <typename T, typename... Args>
    typename std::enable_if<std::is_class<T>::value, void>::type
    pop(std::istringstream &paralist_str, T &data, Args&... args){
        std::string varname;
        std::getline (paralist_str, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << "p:struct:" << (int)get_datatype(data) << ' ' << varname << ' ' << sinfo[varname].width << '\n';
#endif
        override_pop(*this, varname, data);

        pop(paralist_str, args...); // Recursively dump the remaining variables
    }


    //====================================================


    void check(std::istringstream &paralist_str) {
    }

    // Variadic template function to handle multiple variables
    template <typename T, typename... Args>
    typename std::enable_if<!std::is_class<T>::value, void>::type
    check(std::istringstream &paralist_str, T &data, Args&... args){
        std::string varname;
        std::getline (paralist_str, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << "c:" << (int)get_datatype(data) << ' ' << varname << ' ' << sinfo[varname].width << '\n';
#endif
        check_data(varname, data);

        check(paralist_str, args...); // Recursively dump the remaining variables
    }

    template <typename T, typename... Args>
    typename std::enable_if<std::is_class<T>::value, void>::type
    check(std::istringstream &paralist_str, T &data, Args&... args){
        std::string varname;
        std::getline (paralist_str, varname, ',');
        varname = trim_string(varname);
#if DEBUG
        std::cout << "c:struct:" << (int)get_datatype(data) << ' ' << varname << ' ' << sizeof(data)<< '\n';
#endif
        override_pop(*this, varname, data);

        check(paralist_str, args...); // Recursively dump the remaining variables
    }



private:

    std::string trim_string(const std::string& str){
        auto start = str.find_first_not_of(" \t\n\r");
        auto end = str.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }
        return str.substr(start, end - start + 1);
    }

    void pop_data(const std::string &signame, void* ptr)
    {
        auto &siginfo = sinfo.at(signame);
        uint32_t width = siginfo.width;
        for (uint32_t i = 0; i < width; i++) {
            //ptr[cap[qid].width-i]=tgq.front(); 
            char byte = siginfo.content.front();
            ((char*)ptr)[i] = byte;
            //printf("%s %d %d\n", signame.c_str(), i, byte);
            siginfo.content.pop();
        }
    }

    template<typename T>
    void check_data(const std::string& signame, const T& var) {
        T var2;
        pop_data(signame, &var2);
        if (std::memcmp(&var, &var2, sizeof(T)) != 0) {
            std::cout << "tgCheckError: " << signame << " ,sizeOf: " << sizeof(T) << std::endl;

            dump_content("var1", &var, sizeof(T));
            dump_content("var2", &var2, sizeof(T));
            
            exit(-1);
        }
    }

    std::vector<std::string> load_order;
    std::unordered_map<std::string, signal_info_t> sinfo;
    uint32_t test_count;


    void dump_content(const char *s, const void* buf, size_t size){
        printf("%s\n", s);

        const uint8_t *pbuf8 = (const uint8_t *) buf;

        for (size_t i=0; i<size; i++){
            printf("%02x ", pbuf8[i]);

            if (((i+1)%16) == 0) {
                printf("\n");
            } 
        }
        printf("\n");
    }
};

#define tgLoad(filepath)    Ctgload tgload;                                 \
                            if (!tgload.open(filepath)) {                   \
                                printf("Open file %s failed\n", filepath);  \
                                exit(-1);                                   \
                            }

#define tgCount()           tgload.get_count();

#define tgPop(...) do {\
    std::string argvstr = QUOTE(__VA_ARGS__);\
    std::istringstream sn(argvstr);\
    tgload.pop(sn, __VA_ARGS__);\
} while (0)

#define tgCheck(...) do {\
    std::string argvstr = QUOTE(__VA_ARGS__);\
    std::istringstream sn(argvstr);\
    tgload.check(sn, __VA_ARGS__);\
} while (0)


//specialized for struct type
template<typename T>
void override_pop(Ctgload &tgload, const std::string& var_name, T &mv){
    std::cout << "pop func should specialized " << var_name << ',' << sizeof(T) <<'\n';
    exit(-1);
}

#if 1

//specialized for openhevc vector_2d
template<> void override_pop<vector_2d>(Ctgload &tgload, const std::string& var_name, vector_2d &vec)
{
    std::stringstream inner_ss;
    inner_ss << var_name << "_x,";
    inner_ss << var_name << "_y,";

    std::istringstream inner_iss(inner_ss.str());

    tgload.pop(inner_iss, vec.x, vec.y);
}

#endif


template<> void override_pop<child_cmd_t>(Ctgload &tgload, const std::string& var_name, child_cmd_t &cmd)
{
    std::stringstream inner_ss;
    inner_ss << var_name << "_id,";
    inner_ss << var_name << "_param,";

    std::istringstream inner_iss(inner_ss.str());

    tgload.pop(inner_iss, cmd.id, cmd.param);
}