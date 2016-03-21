// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: lisen01@baidu.com
// Brief: lib �������Ļ��࣬���ݻ�׼������ά��tables,���ṩlib������table�ĸ��£����Ҳ�����
// Ĭ��ʵ����load��handle inc����,��ͨ��MVCC�ṩ����ʱ�Ķ�д����

#ifndef DAS_LIB_LIB_MANAGER_BASE_H
#define DAS_LIB_LIB_MANAGER_BASE_H

// Brief: lib������Ļ���
// �������������Ҫʹ�÷���:
// ����TempLibManager����һ������, tmp_mgr��һ��ʵ��
// ��ʼ��: tmp_mgr.init(const comcfg::ConfigUnit& conf)
//      ���õļ��غ������Ա�ĳ�ʼ����TempLibManager::load_conf()�����
//      ���磬TempLibManager::load_conf()�п��ܵĲ�����:
//          ��ȡconf
//          ��ʼ���Լ��ĳ�Ա����
//          ����һ��ͨ���ı���ʽ���صĻ�׼: add_literal_base_conf()
//          ����һ��das������: add_das_inc()
//      �������ڳ�ʼ����ͨ��register_tables()���뵽table group��
// 
// ��ʼ����ɺ�ͨ��start_service()���ػ�׼���Ϳ����ṩ������

#include <cm_utility/file_watcher.h>
#include <cm_utility/align_hash.h>

#include "table_group.h"
#include "das_inc_manager.h"

namespace comcfg {
class ConfigUnit;
}

namespace das_lib {

// forward declaration
class DasIncConf; 

class LibManagerBase {
public:
    LibManagerBase();
    virtual ~LibManagerBase();
    
    template<typename Table>
    Table* get_table(const std::string& name) {
        int pos = _vm_tg.find_latest_read_only();
        if (pos < 0) {
            DL_LOG_FATAL("Failed to find ready on vm");
            return NULL;
        }
        return _vm_tg[pos].get_table<Table>(name);
    }

    bool init(const comcfg::ConfigUnit&);

    bool start_service();

    virtual bool handle_inc();

    TableGroup* latest_table_group();

protected:
    virtual bool load_conf(const comcfg::ConfigUnit&) = 0;

    // ��Ӵ���ص�file watcher ������б�
    void add_file_watcher(cm_utility::FileWatcher* fw);

    // ����һ��das������
    // ע��: ���������������ȼ��ģ�ǰ����ӵ����Ⱥ���ӵ��������ȼ���
    bool add_das_inc(const DasIncConf &das_inc_conf);
    
    // ����һ���ı���׼conf����load�׶Σ����ռ����˳������load�ı���׼
    bool add_literal_base_conf(const DasBaseConf &base_conf);
    
    virtual bool register_tables(TableGroup* table_group) = 0;

private:
    // ��¼ÿ�δ���ĸ����㼶������Ŀ
    typedef cm_utility::align_hash_map<uint32_t,uint32_t> IncCountMap;
    
    //��table group�����µ�version
    //����ֵ: ��version��ָ��
    //@param pos:��version��λ�ã����ں�����freeze����drop����
    TableGroup *create_version(int *pos);
    
    bool load_base_indexes();
    
    // ���ı���ʽ����һ��das���Ļ�׼
    virtual bool load_literal_base(const DasBaseConf &conf,
                                        TableGroup* p_table_group);
    
    //@param record: ��ȡ�ĵ�һ��record��֮�����������������Ϊ����configio��֧����read_nextǰ
    //               ����Ƿ����µ���������Ƿ�������������ж�����ͨ��read_next����
    //               �Ƿ�������������ж�������hand_inc_impl���������Ϊ�漰�Ƿ���Ҫ�����汾�л�
    //@param p_table_group: �����µ�table group
    bool handle_inc_impl(configio::DynamicRecord& record, TableGroup* p_table_group);
    
    void dump_table_if_required();
    void log_handle_inc_info(TableGroup* p_table_group) const;

    st::VersionManager<TableGroup> _vm_tg;

    // �������file watcher�ĵ�ַ�����ڼ���Ƿ���Ҫcreate version
    std::vector<cm_utility::FileWatcher *> _file_watcher_list;

    // ��Ҫͨ���ı���׼���صĻ�׼conf
    std::vector<DasBaseConf> _literal_base_conf_list;

    // �������DAS��
    DasIncManager _inc_manager;

    cm_utility::FileWatcher _dump_req_watcher;    

    IncCountMap _inc_count_map;
    
    static const int TABLE_VERSION_NUM = 2;

};

}  // namespace das_lib

#endif
