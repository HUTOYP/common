// Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
// Author: luanzhaopeng@baidu.com

#ifndef DAS_LIB_TABLE_STRATEGY_H
#define DAS_LIB_TABLE_STRATEGY_H

#include <iostream>
#include <sstream>
#include <string>

#include <cm_utility/file_watcher.h>

#include "lib_util.h"

namespace configio {
class DynamicRecord;
}

namespace das_lib {

//�������ǰ������    
class TableGroup; 

template <class Table>
class IBaseLoadStrategy {
public:
    virtual ~IBaseLoadStrategy() = 0;
    virtual bool init(Table &table) = 0;
    virtual bool pre_load(Table &table) = 0;
    virtual bool load(Table &table) = 0;
    virtual bool post_load(Table &table) = 0;
    virtual bool reload(Table &table) = 0;
    virtual IBaseLoadStrategy *clone(TableGroup *p_table_group) const = 0;
};

//���ڵ��ŵĴ���
//ConnArg �����Ӳ���ģ�壬Ĭ��֧��һ�����Ӳ����������Ҫ�������
//���Խ�ʹ�ýṹ����Ϊ��������
template <class Table, class Connector, typename ConnArg = int>
class ConnectorLoadStrategy : public IBaseLoadStrategy<Table> {
public:
    //����connector�ķ�����ע����Ҫ�ڷ���ǰ����connect()����
    typedef Connector *(*ConnectorMaker)(Table &table, TableGroup &table_group, ConnArg);

    ConnectorLoadStrategy(const std::string &desc,
                                ConnectorMaker connector_maker,
                                TableGroup *p_table_group,
                                ConnArg connect_arg);

    virtual ~ConnectorLoadStrategy();
    virtual bool init(Table &table);
    virtual bool pre_load(Table &table) ;
    virtual bool load(Table &table);
    virtual bool post_load(Table &table);
    virtual bool reload(Table &table);
    virtual ConnectorLoadStrategy *clone(TableGroup *p_table_group) const;

private:
    ConnectorLoadStrategy(const ConnectorLoadStrategy &rhs);
    void enable_connector(Table &table);
    void disable_connector(Table &table);
    void show_connector() const;

    const std::string _conn_desc;
    Connector *_p_connector;       //own this object
    ConnectorMaker _connector_maker;
    ConnArg _connect_arg;
    TableGroup *_p_table_group;    // not own this object
};

/*
brief:
ͨ����ȡ�����ļ�������smalltable�Ĳ�����
��һ��λ�ڸò������ⲿ��file watcher����������ļ��ı仯��
��������ļ����£���Ҫ���ε��ø����pre_load()��load()��post_load()����
����ɱ��reload��

ʹ���߿����ṩpre_load��post_load����handler���������Ҫ������NULL���ɣ�
һ��ģ�pre_load�Ĳ����ǽ��ò��Թ���ı��漰�����������ŵ��������Ͽ���
�Ա���ñ����¼��ص��´��������Ӳ�����
post_load�н��лָ��������Ĳ�����

��file watcher�������ⲿ��ԭ����lib manager��Ҫ���file watcher���ж��Ƿ���Ҫ
����table group��creat version������
file watcher�ĸ�����reload()��������ɣ�
*/
template <class Table>
class FileLoadStrategy : public IBaseLoadStrategy<Table> {
public:
    typedef bool (*PreLoader)(TableGroup &);
    typedef bool (*Loader)(Table &, const char*, const PartitionArg *);
    typedef bool (*PostLoader)(TableGroup &);
    
    FileLoadStrategy(const PartitionArg *p_part_arg,
                        PreLoader pre_loader,
                        Loader loader,
                        PostLoader post_loader,
                        cm_utility::FileWatcher &_file_watcher,
                        TableGroup *p_table_group
                        );

    virtual bool init(Table &table);
    virtual bool pre_load(Table &table);
    virtual bool load(Table &table);
    virtual bool post_load(Table &table);
    virtual bool reload(Table &table);
    virtual FileLoadStrategy *clone(TableGroup *p_table_group) const;

private:
    FileLoadStrategy(const FileLoadStrategy &rhs);

    const PartitionArg *_p_part_arg;    //maybe NULL when there is no partition need

    const PreLoader _pre_load_handler;
    const Loader _load_handler;
    const PostLoader _post_load_handler;

    cm_utility::FileWatcher &_file_watcher; // not own this object
    TableGroup *_p_table_group;    // not own this object
};

template <class Table>
class IBaseUpdateStrategy {
public:
    virtual ~IBaseUpdateStrategy() = 0;
                
    virtual bool init(Table &table) = 0;
    virtual bool update(Table &table, const configio::DynamicRecord &) = 0;
    virtual IBaseUpdateStrategy *clone() const = 0;
};

template <class Table>
class IncUpdateStrategy : public IBaseUpdateStrategy<Table> {
public:
    typedef bool (*UpdateHandler)(Table &table, const configio::DynamicRecord &);
    
    explicit IncUpdateStrategy(UpdateHandler update_handler)
        : _update_handler(update_handler)
    {}

    virtual bool init(Table &table);
    virtual bool update(Table &table, const configio::DynamicRecord &);
    virtual IncUpdateStrategy *clone() const;
    
private:
    UpdateHandler _update_handler;
};

} //namespace das-lib

#include "table_strategy_inl.hpp"

#endif
