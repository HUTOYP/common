// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com
// Brief: ʹ��configio���������£�smalltable��Ϊ�ײ�洢��������
#ifndef DAS_LIB_TABLE_MANAGER_H
#define DAS_LIB_TABLE_MANAGER_H

#include "table_strategy.hpp"

namespace configio {
class DynamicRecord;
}
 
namespace das_lib {

class TableGroup;    

class IBaseTableManager {
public:
    virtual ~IBaseTableManager(){}
    virtual bool init() = 0;
    virtual bool pre_load() = 0;
    virtual bool load() = 0;
    virtual bool post_load() = 0;
    virtual bool update(const DynamicRecord &inc_record) = 0;
    virtual bool reload() = 0;

    //��ͬ�汾��TableGroup��ͬ�������Ҫ��cloneʱ����
    virtual IBaseTableManager *clone(TableGroup *pTable_group) const = 0;
    virtual IBaseTableManager &operator=(const IBaseTableManager &) = 0;

    virtual size_t mem() const = 0;
    virtual const std::string &desc() const = 0;
    virtual size_t size() const = 0;
    virtual void serialize(const char *path) const = 0;
};

//smalltable�����࣬���Զ��Ʋ�ͬ�ļ��غ͸��²���
//�����ı���׼���ص����ű�ʹ���������²��ԣ�����Ҫָ�����ز���
//connector���ɵĵ��ű�ʹ��connector���ز��ԣ�����Ҫָ�����²���
//���ڼ��غ͸��²��Զ�����Ϊ�գ����û�м��ز��ԣ�pre_load()/load()/post_load()ֱ�ӷ���true
//���û�и��²��ԣ�update()ֱ�ӷ���true
template <class Table>
class TableManager : public IBaseTableManager {
public:
    TableManager(const std::string &desc,
                    IBaseLoadStrategy<Table> *p_load_strategy,
                    IBaseUpdateStrategy<Table> *p_update_strategy,
                    TableGroup *pTable_group);
    virtual ~TableManager();

    virtual bool init();
    virtual bool pre_load();
    virtual bool load();
    virtual bool post_load();
    virtual bool update(const DynamicRecord &inc_record);
    virtual bool reload();
    
    // brief: allocate a TableManager of the same type, 
    //        clone the load and update strategy and init them
    //        set its table group pointer using the parmater
    virtual TableManager *clone(TableGroup *pTable_group) const;

    // note: is meant to be used by version table manager
    // so, table name, load & update strategies will not be copied
    // only copy the small table
    virtual TableManager &operator=(const IBaseTableManager &rhs);

    virtual const std::string &desc() const;
    virtual size_t mem() const;
    virtual size_t size() const ;
    virtual void serialize(const char *path) const;
    virtual void dump_info() const;

    Table *mutable_table();
    const Table &get_table() const;

private:
    TableManager();
    TableManager(const TableManager &rhs);
    TableManager &operator=(const TableManager &rhs);
   
    Table _table;  
    const std::string _desc;
    IBaseLoadStrategy<Table> *_p_load_strategy;     //own this object
    IBaseUpdateStrategy<Table> *_p_update_strategy; //own this object
    TableGroup *_p_table_group;    //not own this object

    static const uint32_t RESERVE_SIZE = 100000;
};

} //namespace das lib

#include "table_manager_inl.hpp"

#endif
