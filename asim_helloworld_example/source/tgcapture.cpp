#include <vector>
#include "tgcapture.h"


//static CCapture member variable
std::unordered_set<std::string> CCapture::warn_msg;
std::string CCapture::parent_func;


//point to CCapture in the the parent function
CCapture *g_capture;

//Store the pointer of all the CCapture.
//if parent function is finished, it will iterately loop through each CCapture->close() to save the captured file
std::vector<CCapture*> capture_vec;


//specialized for struct type
template<typename T>
void override_capture(CCapture &cap, enum CCapture::CAPTURE_STATUS mode, const std::string& var_name, T &mv){
    std::cout << "capture func should specialized " << var_name << ',' << sizeof(T) <<'\n';
    exit(-1);
}

#if 1

//specialized for vector_2d
template<> void override_capture<const vector_2d>(CCapture &cap, enum CCapture::CAPTURE_STATUS mode, const std::string& var_name,  const vector_2d &vec)
{
    if (mode == CCapture::ON_OPEN){
        std::stringstream inner_ss;
        inner_ss << var_name << "_x,";
        inner_ss << var_name << "_y,";

        std::istringstream inner_iss(inner_ss.str());

        cap.init_var(inner_iss, vec.x, vec.y);

    }
    else if (mode == CCapture::ON_CAPTURE) {
        std::stringstream inner_ss;

        inner_ss << var_name << "_x,";
        inner_ss << var_name << "_y,";

        std::istringstream inner_iss(inner_ss.str());

        cap.capture(false, inner_iss, vec.x, vec.y);
    }
}


#endif


template<> void override_capture<const child_cmd_t>(CCapture &cap, enum CCapture::CAPTURE_STATUS mode, const std::string& var_name,  const child_cmd_t &cmd)
{
    if (mode == CCapture::ON_OPEN){
        std::stringstream inner_ss;
        inner_ss << var_name << "_id,";
        inner_ss << var_name << "_param,";

        std::istringstream inner_iss(inner_ss.str());

        cap.init_var(inner_iss, cmd.id, cmd.param);

    }
    else if (mode == CCapture::ON_CAPTURE) {
        std::stringstream inner_ss;

        constexpr size_t CMD_PARAM_ARRAY_NUM = sizeof(cmd.param)/sizeof(cmd.param[0]);

        inner_ss << var_name << "_id,";
        inner_ss << var_name << "_param," << CMD_PARAM_ARRAY_NUM << ',' ;

        std::istringstream inner_iss(inner_ss.str());

        cap.capture(false, inner_iss, cmd.id, cmd.param, CMD_PARAM_ARRAY_NUM);
    }
}


