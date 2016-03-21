// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com

#ifndef DAS_LIB_DAS_INC_MANAGER_H
#define DAS_LIB_DAS_INC_MANAGER_H

#include <stdint.h>
#include <string>
#include <vector>

#include <cm_utility/non_copyable.h>

#include "das_inc_reader.h"     // for DasIncConf

namespace configio {
class DynamicRecord;
}

namespace das_lib {

// ����DAS�������Ĺ����࣬ʹ������׷����ʱֻ��Ҫ����read_next()�ӿ�
class DasIncManager {
public:
    DasIncManager() {}
    ~DasIncManager();
    
    // ������ӵ�˳�����ζ�ȡ��������������
    // ��һ����������ȫ������������֮��Ŷ�ȡ��һ��������
    int read_next(configio::DynamicRecord *p_record);

    bool add_das_inc(const DasIncConf &das_inc_conf);

    void set_max_reading_lines_for_all_inc(uint64_t num);

    // ����inc reader��ÿ�ֶ�ȡ�������ó�confָ��ֵ
    void reset_max_reading_lines_for_all_inc();
    
private:
    struct DasIncObject {
        DASIncReader *p_inc_reader;   // own this object
        DasIncConf conf;
        std::string name;
    };

    DISALLOW_COPY_AND_ASSIGN(DasIncManager);

    void set_max_lines_per_round(uint32_t index, uint64_t num);

    //����das������
    std::vector<DasIncObject> _das_inc_list;

};

} // das_lib
#endif // DAS_LIB_DAS_INC_MANAGER_H

