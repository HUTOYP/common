/**
 * ͨ�������ļ���ȡ
 */
#ifndef __INI_CONFIGURATION_H__
#define __INI_CONFIGURATION_H__

#include <mem_db/common/ini_parser.h>
#include <string>

namespace mem_db_util {

/**
 * ��ȡ�����ļ�
 */
class CINIConfig {
  public:
    CINIConfig ();
    CINIConfig (const char *profile);
    CINIConfig (const std::string &profile);
    virtual ~CINIConfig ();
    /**
     * ������������
     * @param areaName ��������
     */
    void setAreaName (const char *areaName);

    void parse (const std::string &profile);

    /**
     * ȡһ������ֵ
     * @param key  ����������
     * @param def  ��ָ�������������ʱ���ص�ȱʡֵ
     * @return ����ֵ
     */
    int getInt (const char *key, int def = 0)const;
    int getInt (const std::string &key, int def = 0)const;

    /**
     * ȡһ���Ǹ�����ֵ
     * @param key  ����������
     * @param def  ��ָ�������������ʱ���ص�ȱʡֵ
     * @return ����ֵ
     */
    unsigned getUInt (const char *key, unsigned def = 0)const;
    unsigned getUInt (const std::string &key, unsigned def = 0)const;

    /**
     * ȡһ���ַ���
     * @param key  ����������
     * @param def  ��ָ�������������ʱ���ص�ȱʡֵ
     * @return ����ֵ
     */
    const char *getString (const char *key, const char *def = NULL)const;
    const char *getString (const std::string &key, const char *def = NULL)const;

    /**
     * ȡһ������ֵ
     * @param key  ����������
     * @param def  ��ָ�������������ʱ���ص�ȱʡֵ
     * @return ����ֵ
     */
    bool getBool (const char *key, bool def = false)const;
    bool getBool (const std::string &key, bool def = false)const;

  protected:
    INIParser m_ini;

    std::string m_areaName;
    const char *m_areaName_c;
};

}

#endif

