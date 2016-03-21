#ifndef _IPLUGIN_H_
#define _IPLUGIN_H_

class iplugin_t {
  public:
    virtual ~iplugin_t() {};
  public:
    //�����ʼ��
    virtual int initialize(const char *configure_file) = 0;
    //�������ʼ��
    virtual int uninitialize() = 0;
    //���¼�������, ���ʹ��configure_file = NULL,ʹ��initializeʱ�����initialize
    virtual int reload_cfg(const char *configure_file = NULL) = 0;
    //��ȡͳ����Ϣ
    virtual int statistics(char *buffer, size_t &len) = 0;
};

#endif
