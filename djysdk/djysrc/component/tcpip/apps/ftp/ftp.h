//-----------------------------------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 ����Ȩ����������������޹�˾���С�����Ȩ�˱���һ��Ȩ����
//
// �����Ȩ�����ʹ���߷��������������������£�����ʹ����ʹ�ü���ɢ����
// �����װԭʼ�뼰����λ��ִ����ʽ��Ȩ�������۴˰�װ�Ƿ񾭸�����Ȼ��
//
// 1. ���ڱ����Դ�������ɢ�������뱣�������İ�Ȩ���桢�������б���
//    ������������������
// 2. ���ڱ��׼�����λ��ִ����ʽ����ɢ���������������ļ��Լ�������������
//    ��ɢ����װ�е�ý�鷽ʽ����������֮��Ȩ���桢�������б��Լ�����
//    ������������
// 3. �������Ϊ�����߲���ϵͳ����ɲ��֣�δ����ǰȡ�õ�������ɣ���������ֲ����
//    �����߲���ϵͳ���������С�

// ����������������Ǳ������Ȩ�������Լ�����������״��"as is"���ṩ��
// �������װ�����κ���ʾ��Ĭʾ֮�������Σ������������ھ��������Լ��ض�Ŀ
// �ĵ�������ΪĬʾ�Ե�������Ȩ�����˼������֮�����ߣ������κ�������
// ���۳�����κ��������塢���۴�����Ϊ���Լ��ϵ���޹�ʧ������������Υ
// Լ֮��Ȩ��������ʧ������ԭ��ȣ����𣬶����κ���ʹ�ñ������װ��������
// �κ�ֱ���ԡ�����ԡ�ż���ԡ������ԡ��ͷ��Ի��κν�����𺦣�����������
// �������Ʒ������֮���á�ʹ����ʧ��������ʧ��������ʧ��ҵ���жϵȵȣ���
// �����κ����Σ����ڸ���ʹ���ѻ���ǰ��֪���ܻ���ɴ����𺦵���������Ȼ��
//-----------------------------------------------------------------------------

#ifndef __FTP_H
#define __FTP_H

#define CN_FILEREBOOT                 110	//���ļ�ָʾ���ϵ��������
#define CN_SERVERREADYTIME            120	//������׼��������ʱ�䣨��������
#define CN_OPENDATACONNECTION125      125	//���������ӣ���ʼ����
#define CN_OPENCONNECTION             150	//������
#define CN_CMDSUCCESS                 200	//�ɹ�
#define CN_CMDFAILED                  202	//����û��ִ��
#define CN_SYSSTAT                    211	//ϵͳ״̬�ظ�
#define CN_DIRSTAT                    212	//Ŀ¼״̬�ظ�
#define CN_FILESTAT                   213	//�ļ�״̬�ظ�
#define CN_HELPSTAT                   214	//������Ϣ�ظ�
#define CN_SYSTYPE                    215	//ϵͳ���ͻظ�
#define CN_SERVERREADY                220	//�������
#define CN_LOGOUT                     221	//�˳�����
#define CN_OPENDATACONNECTION225      225	//����������
#define CN_CLOSEDATACONNECTION        226	//������������
#define CN_ENTERPASSIVE               227	//���뱻��ģʽ��IP ��ַ��ID �˿ڣ�
#define CN_LOGIN                      230	//��¼������
#define CN_FILECOMPLETE               250	//�ļ���Ϊ���
#define CN_PATHBUILD                  257	//·��������
#define CN_NEEDPASS                   331	//Ҫ������
#define CN_NEEDUSER                   332	//Ҫ���ʺ�
#define CN_FILEPAUSE                  350	//�ļ���Ϊ��ͣ
#define CN_SERVERCLOSE                421	//����ر�
#define CN_OPENDATACONNECTIONFAILED   425	//�޷�����������
#define CN_CLOSECONNECTION            426	//��������
#define CN_FILENOTVALID               450	//�ļ�������
#define CN_LOCALERR                   451	//�������ش���
#define CN_SPACENOTENOUGH             452	//���̿ռ䲻��
#define CN_CMDINVALID                 500	//��Ч����
#define CN_PARAERR                    501	//�������
#define CN_CMDNOTEXE                  502	//����û��ִ��
#define CN_CMDORDERERR                503	//����ָ������
#define CN_CMDPARAINVALID             504	//��Ч�������
#define CN_NOLOG                      530	//δ��¼����
#define CN_STRNEEDUSER                532	//�洢�ļ���Ҫ�ʺ�
#define CN_FILEINVALID                550	//�ļ�������
#define CN_UNKOWNPAGE                 551	//��֪����ҳ����
#define CN_EXCEEDSTORAGED             552	//�����洢����
#define CN_FILENAMEDENI               553	//�ļ���������

#endif /* __FTP_H */
