#include "dxa.h"

#include <cstdio>
#include <fstream>
#include <vector>
#include <filesystem>

// Codes are from DXArchive, with some modification
// Original author: ɽ�� ��
// Homepage: https://dxlib.xsrv.jp/dxtec.html

namespace dxa
{

#define MAX_PATH 260
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

const u32   MIN_COMPRESS_VER5 = 4;

const u16	DXA_HEAD_VER5			= *(u16*)("DX");// �إå�
const u16	DXA_VER_VER5			= (0x0005);		// �Щ`�����
const u32	DXA_BUFFERSIZE_VER5		= (0x1000000);	// ���`���������ɕr��ʹ�ä���Хåե��Υ�����
const u32	DXA_KEYSTR_LENGTH_VER5	= (12);			// �I�����Ф��L��

#pragma pack(push)
#pragma pack(1)

// ���`�����֥ǩ`��������Υإå�
typedef struct tagDARC_HEAD_VER5
{
	u16 Head;								// �إå�
	u16 Version;							// �Щ`�����
	u32 HeadSize;							// �إå����� DARC_HEAD_VER5 ��i����ȫ������
	u32 DataStartAddress;					// ����Υե�����Υǩ`������{����Ƥ���ǩ`�����ɥ쥹(�ե���������^���ɥ쥹�򥢥ɥ쥹���Ȥ���)
	u32 FileNameTableStartAddress;			// �ե��������Ʃ`�֥�����^���ɥ쥹(�ե���������^���ɥ쥹�򥢥ɥ쥹���Ȥ���)
	u32 FileTableStartAddress;				// �ե�����Ʃ`�֥�����^���ɥ쥹(���Љ��� FileNameTableStartAddress �Υ��ɥ쥹�򣰤Ȥ���)
	u32 DirectoryTableStartAddress;		// �ǥ��쥯�ȥ�Ʃ`�֥�����^���ɥ쥹(���Љ��� FileNameTableStartAddress �Υ��ɥ쥹�򣰤Ȥ���)
											// ���ɥ쥹���������ä���Ƥ��� DARC_DIRECTORY_VER5 �����夬��`�ȥǥ��쥯�ȥ�
	u32 CodePage;							// �ե���������ʹ�ä��Ƥ��륳�`�ɥک`������
} DARC_HEAD_VER5;

// ���`�����֥ǩ`��������Υإå�(Ver 0x0003�ޤ�)
typedef struct tagDARC_HEAD_VER3
{
	u16 Head;								// �إå�
	u16 Version;							// �Щ`�����
	u32 HeadSize;							// �إå����� DARC_HEAD_VER5 ��i����ȫ������
	u32 DataStartAddress;					// ����Υե�����Υǩ`������{����Ƥ���ǩ`�����ɥ쥹(�ե���������^���ɥ쥹�򥢥ɥ쥹���Ȥ���)
	u32 FileNameTableStartAddress;			// �ե��������Ʃ`�֥�����^���ɥ쥹(�ե���������^���ɥ쥹�򥢥ɥ쥹���Ȥ���)
	u32 FileTableStartAddress;				// �ե�����Ʃ`�֥�����^���ɥ쥹(���Љ��� FileNameTableStartAddress �Υ��ɥ쥹�򣰤Ȥ���)
	u32 DirectoryTableStartAddress;		// �ǥ��쥯�ȥ�Ʃ`�֥�����^���ɥ쥹(���Љ��� FileNameTableStartAddress �Υ��ɥ쥹�򣰤Ȥ���)
											// ���ɥ쥹���������ä���Ƥ��� DARC_DIRECTORY_VER5 �����夬��`�ȥǥ��쥯�ȥ�
} DARC_HEAD_VER3;

// �ե�����Εr�g���
typedef struct tagDARC_FILETIME_VER5
{
	u64 Create;			// ���ɕr�g
	u64 LastAccess;		// ��K���������r�g
	u64 LastWrite;			// ��K���r�g
} DARC_FILETIME_VER5;

// �ե������{���(Ver 0x0001)
typedef struct tagDARC_FILEHEAD_VER1
{
	u32 NameAddress;			// �ե�����������{����Ƥ��륢�ɥ쥹( ARCHIVE_HEAD������ �Υ��Љ��� FileNameTableStartAddress �Υ��ɥ쥹�򥢥ɥ쥹���Ȥ���) 

	u32 Attributes;			// �ե���������
	DARC_FILETIME_VER5 Time;	// �r�g���
	u32 DataAddress;			// �ե����뤬��{����Ƥ��륢�ɥ쥹
								//			�ե�����Έ��ϣ�DARC_HEAD_VER5������ �Υ��Љ��� DataStartAddress ��ʾ�����ɥ쥹�򥢥ɥ쥹���Ȥ���
								//			�ǥ��쥯�ȥ�Έ��ϣ�DARC_HEAD_VER5������ �Υ��Љ��� DirectoryTableStartAddress �Τ�ʾ�����ɥ쥹�򥢥ɥ쥹���Ȥ���
	u32 DataSize;				// �ե�����Υǩ`��������
} DARC_FILEHEAD_VER1;

// �ե������{���
typedef struct tagDARC_FILEHEAD_VER5
{
	u32 NameAddress;			// �ե�����������{����Ƥ��륢�ɥ쥹( ARCHIVE_HEAD������ �Υ��Љ��� FileNameTableStartAddress �Υ��ɥ쥹�򥢥ɥ쥹���Ȥ���) 

	u32 Attributes;			// �ե���������
	DARC_FILETIME_VER5 Time;	// �r�g���
	u32 DataAddress;			// �ե����뤬��{����Ƥ��륢�ɥ쥹
								//			�ե�����Έ��ϣ�DARC_HEAD_VER5������ �Υ��Љ��� DataStartAddress ��ʾ�����ɥ쥹�򥢥ɥ쥹���Ȥ���
								//			�ǥ��쥯�ȥ�Έ��ϣ�DARC_HEAD_VER5������ �Υ��Љ��� DirectoryTableStartAddress �Τ�ʾ�����ɥ쥹�򥢥ɥ쥹���Ȥ���
	u32 DataSize;				// �ե�����Υǩ`��������
	u32 CompressedDataSize;			// �R�s��Υǩ`���Υ�����( 0xffffffff:�R�s����Ƥ��ʤ� ) ( Ver0x0002 ��׷�Ӥ��줿 )
} DARC_FILEHEAD_VER5;

// �ǥ��쥯�ȥ��{���
typedef struct tagDARC_DIRECTORY_VER5
{
	u32 DirectoryAddress;			// �Է֤� DARC_FILEHEAD_VER5 ����{����Ƥ��륢�ɥ쥹( DARC_HEAD_VER5 ������ �Υ��Љ��� FileTableStartAddress ��ʾ�����ɥ쥹�򥢥ɥ쥹���Ȥ���)
	u32 ParentDirectoryAddress;	// �H�ǥ��쥯�ȥ�� DARC_DIRECTORY_VER5 ����{����Ƥ��륢�ɥ쥹( DARC_HEAD_VER5������ �Υ��Љ��� DirectoryTableStartAddress ��ʾ�����ɥ쥹�򥢥ɥ쥹���Ȥ���)
	u32 FileHeadNum;				// �ǥ��쥯�ȥ��ڤΥե��������
	u32 FileHeadAddress;			// �ǥ��쥯�ȥ��ڤΥե�����Υإå��Ф���{����Ƥ��륢�ɥ쥹( DARC_HEAD_VER5������ �Υ��Љ��� FileTableStartAddress ��ʾ�����ɥ쥹�򥢥ɥ쥹���Ȥ���) 
} DARC_DIRECTORY_VER5;

#pragma pack(pop)

// �ե��������ǩ`������Ԫ�Υե��������������Ф�ȡ�ä���
const char* GetOriginalFileName(u8* FileNameTable)
{
	return (char*)FileNameTable + *((u16*)&FileNameTable[0]) * 4 + 4;
}

// �I�����Ф�����
void KeyCreate(const char* Source, unsigned char* Key)
{
	int Len;

	if (Source == NULL)
	{
		memset(Key, 0xaaaaaaaa, DXA_KEYSTR_LENGTH_VER5);
	}
	else
	{
		Len = strlen(Source);
		if (Len > DXA_KEYSTR_LENGTH_VER5)
		{
			memcpy(Key, Source, DXA_KEYSTR_LENGTH_VER5);
		}
		else
		{
			// �I�����Ф� DXA_KEYSTR_LENGTH_VER5 ���̤��ä����`�פ���
			int i;

			for (i = 0; i + Len <= DXA_KEYSTR_LENGTH_VER5; i += Len)
				memcpy(Key + i, Source, Len);
			if (i < DXA_KEYSTR_LENGTH_VER5)
				memcpy(Key + i, Source, DXA_KEYSTR_LENGTH_VER5 - i);
		}
	}

	Key[0] = ~Key[0];
	Key[1] = (Key[1] >> 4) | (Key[1] << 4);
	Key[2] = Key[2] ^ 0x8a;
	Key[3] = ~((Key[3] >> 4) | (Key[3] << 4));
	Key[4] = ~Key[4];
	Key[5] = Key[5] ^ 0xac;
	Key[6] = ~Key[6];
	Key[7] = ~((Key[7] >> 3) | (Key[7] << 5));
	Key[8] = (Key[8] >> 5) | (Key[8] << 3);
	Key[9] = Key[9] ^ 0x7f;
	Key[10] = ((Key[10] >> 4) | (Key[10] << 4)) ^ 0xd6;
	Key[11] = Key[11] ^ 0xcc;
}

// �I�����Ф�ʹ�ä��� Xor ����( Key �ϱؤ� DXA_KEYSTR_LENGTH_VER5 ���L�����ʤ���Фʤ�ʤ� )
void KeyConv(void* Data, int Size, int Position, unsigned char* Key)
{
	Position %= DXA_KEYSTR_LENGTH_VER5;

	int i, j;

	j = Position;
	for (i = 0; i < Size; i++)
	{
		((u8*)Data)[i] ^= Key[j];

		j++;
		if (j == DXA_KEYSTR_LENGTH_VER5) j = 0;
	}
}

// �ե����뤫���i���z����ǩ`�����I�����Ф�ʹ�ä��� Xor ���㤹���v��( Key �ϱؤ� DXA_KEYSTR_LENGTH_VER5 ���L�����ʤ���Фʤ�ʤ� )
void KeyConvFileRead(void* Data, int Size, std::ifstream& fp, unsigned char* Key, int Position = -1)
{
	int pos;

	// �ե������λ�ä�ȡ�ä��Ƥ���
	if (Position == -1)  pos = fp.tellg();
	else                 pos = Position;

	// �i���z��
	fp.read((char*)Data, Size);

	// �ǩ`�����I�����Ф�ʹ�ä� Xor ����
	KeyConv(Data, Size, pos, Key);
}

// �ǥ��`��( ���ꂎ:�����Υ�����  -1 �ϥ���`  Dest �� NULL �����뤳�Ȥ���� )
int Decompress(void* Src, void* Dest)
{
	u32 srcsize, destsize, code, indexsize, keycode, conbo, index;
	u8* srcp, * destp, * dp, * sp;

	destp = (u8*)Dest;
	srcp = (u8*)Src;

	// �����Υǩ`����������ä�
	destsize = *((u32*)&srcp[0]);

	// �R�s�ǩ`���Υ�������ä�
	srcsize = *((u32*)&srcp[4]) - 9;

	// ���`���`��
	keycode = srcp[8];

	// �����Ȥ��ʤ����Ϥϥ�������������
	if (Dest == NULL)
		return destsize;

	// չ�_�_ʼ
	sp = srcp + 9;
	dp = destp;
	while (srcsize)
	{
		// ���`���`�ɤ�ͬ���ǄI�����
		if (sp[0] != keycode)
		{
			// �ǈR�s���`�ɤΈ��ϤϤ��Τޤ޳���
			*dp = *sp;
			dp++;
			sp++;
			srcsize--;
			continue;
		}

		// ���`���`�ɤ��B�A���Ƥ������Ϥϥ��`���`����������
		if (sp[1] == keycode)
		{
			*dp = (u8)keycode;
			dp++;
			sp += 2;
			srcsize -= 2;

			continue;
		}

		// ��һ�Х��Ȥ�ä�
		code = sp[1];

		// �⤷���`���`�ɤ���󤭤ʂ����ä����Ϥϥ��`���`��
		// �ȤΥХåƥ��󥰷�ֹ�Ξ�ˣ������Ƥ���Τǣ�������
		if (code > keycode) code--;

		sp += 2;
		srcsize -= 2;

		// �B�A�L��ȡ�ä���
		conbo = code >> 3;
		if (code & (0x1 << 2))
		{
			conbo |= *sp << 5;
			sp++;
			srcsize--;
		}
		conbo += MIN_COMPRESS_VER5;	// ����r�˜p�㤷����С�R�s�Х��������㤹

		// �����������ɥ쥹��ȡ�ä���
		indexsize = code & 0x3;
		switch (indexsize)
		{
		case 0:
			index = *sp;
			sp++;
			srcsize--;
			break;

		case 1:
			index = *((u16*)sp);
			sp += 2;
			srcsize -= 2;
			break;

		case 2:
			index = *((u16*)sp) | (sp[2] << 16);
			sp += 3;
			srcsize -= 3;
			break;
		}
		index++;		// ����r�ˣ������Ƥ���Τǣ�������

		// չ�_
		if (index < conbo)
		{
			u32 num;

			num = index;
			while (conbo > num)
			{
				memcpy(dp, dp - num, num);
				dp += num;
				conbo -= num;
				num += num;
			}
			if (conbo != 0)
			{
				memcpy(dp, dp - num, conbo);
				dp += conbo;
			}
		}
		else
		{
			memcpy(dp, dp - index, conbo);
			dp += conbo;
		}
	}

	// �����Υ������򷵤�
	return (int)destsize;
}

// ָ���Υǥ��쥯�ȥ�ǩ`���ˤ���ե������չ�_����
int DirectoryDecode(u8* NameP, u8* DirP, u8* FileP, DARC_HEAD_VER5* Head, DARC_DIRECTORY_VER5* Dir, std::ifstream& ArcP, unsigned char* Key, Path DirPath, DXArchive& output)
{
	// �ǥ��쥯�ȥ���󤬤�����Ϥϡ��ޤ�չ�_�äΥǥ��쥯�ȥ�����ɤ���
	if (Dir->DirectoryAddress != 0xffffffff && Dir->ParentDirectoryAddress != 0xffffffff)
	{
		DARC_FILEHEAD_VER5* DirFile;

		// DARC_FILEHEAD_VER5 �Υ��ɥ쥹��ȡ��
		DirFile = (DARC_FILEHEAD_VER5*)(FileP + Dir->DirectoryAddress);

		DirPath /= GetOriginalFileName(NameP + DirFile->NameAddress);
	}

	// չ�_�I���_ʼ
	{
		u32 i, FileHeadSize;
		DARC_FILEHEAD_VER5* File;

		// ��{����Ƥ���ե�������������R�귵��
		FileHeadSize = Head->Version >= 0x0002 ? sizeof(DARC_FILEHEAD_VER5) : sizeof(DARC_FILEHEAD_VER1);
		File = (DARC_FILEHEAD_VER5*)(FileP + Dir->FileHeadAddress);
		for (i = 0; i < Dir->FileHeadNum; i++, File = (DARC_FILEHEAD_VER5*)((u8*)File + FileHeadSize))
		{
			// �ǥ��쥯�ȥ꤫�ɤ����ǄI�����
			if (File->Attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// �ǥ��쥯�ȥ�Έ��Ϥ��َ��򤫤���
				DirectoryDecode(NameP, DirP, FileP, Head, (DARC_DIRECTORY_VER5*)(DirP + File->DataAddress), ArcP, Key, DirPath, output);
			}
			else
			{
				DXArchiveSegment DestP;
				//DestP.path = (DirPath / GetOriginalFileName(NameP + File->NameAddress)).string();
				//DestP.size = DXA_BUFFERSIZE_VER5;
				//DestP.data.assign(DestP.size, 0);

				// �ǩ`����������ϤΤ�ܞ��
				if (File->DataSize != 0)
				{
					// ����λ�ä򥻥åȤ���
					if (ArcP.tellg() != (Head->DataStartAddress + File->DataAddress))
						ArcP.seekg(Head->DataStartAddress + File->DataAddress);

					// �ǩ`�����R�s����Ƥ��뤫�ɤ����ǄI�����
					if (Head->Version >= 0x0002 && File->CompressedDataSize != 0xffffffff)
					{
						void* temp;

						// �R�s����Ƥ������

						// �R�s�ǩ`�������ޤ�����I��δ_��
						temp = malloc(File->CompressedDataSize);

						// �R�s�ǩ`�����i���z��
						if (Head->Version >= 0x0005)
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key, File->DataSize);
						}
						else
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key);
						}

						// ���
						DestP.size = File->DataSize;
						DestP.data = std::shared_ptr<uint8_t>(new uint8_t[DestP.size]);
						Decompress(temp, &*DestP.data);

						// ����ν��
						free(temp);
					}
					else
					{
						// �R�s����Ƥ��ʤ�����

						// ܞ�̈́I���_ʼ
						{
							u32 MoveSize, WriteSize;

							WriteSize = 0;
							DestP.size = File->DataSize;
							DestP.data = std::shared_ptr<uint8_t>(new uint8_t[DestP.size]);
							while (WriteSize < File->DataSize)
							{
								MoveSize = File->DataSize - WriteSize > DXA_BUFFERSIZE_VER5 ? DXA_BUFFERSIZE_VER5 : File->DataSize - WriteSize;

								// �ե�����η�ܞ�i���z��
								if (Head->Version >= 0x0005)
								{
									KeyConvFileRead(&*DestP.data + WriteSize, MoveSize, ArcP, Key, File->DataSize + WriteSize);
								}
								else
								{
									KeyConvFileRead(&*DestP.data + WriteSize, MoveSize, ArcP, Key);
								}

								WriteSize += MoveSize;
							}
						}
					}
				}

				output[(DirPath / GetOriginalFileName(NameP + File->NameAddress)).string()] = DestP;
			}
		}
	}

	// �K��
	return 0;
}

// ָ���Υǥ��쥯�ȥ�ǩ`���ˤ���ե������չ�_����
int DirectoryDecode(u8* NameP, u8* DirP, u8* FileP, DARC_HEAD_VER5* Head, DARC_DIRECTORY_VER5* Dir, std::ifstream& ArcP, unsigned char* Key, Path DirPath)
{
	if (!std::filesystem::exists(DirPath))
		std::filesystem::create_directories(DirPath);

	// �ǥ��쥯�ȥ���󤬤�����Ϥϡ��ޤ�չ�_�äΥǥ��쥯�ȥ�����ɤ���
	if (Dir->DirectoryAddress != 0xffffffff && Dir->ParentDirectoryAddress != 0xffffffff)
	{
		DARC_FILEHEAD_VER5* DirFile;

		// DARC_FILEHEAD_VER5 �Υ��ɥ쥹��ȡ��
		DirFile = (DARC_FILEHEAD_VER5*)(FileP + Dir->DirectoryAddress);

		DirPath /= GetOriginalFileName(NameP + DirFile->NameAddress);
	}

	// չ�_�I���_ʼ
	{
		u32 i, FileHeadSize;
		DARC_FILEHEAD_VER5* File;

		// ��{����Ƥ���ե�������������R�귵��
		FileHeadSize = Head->Version >= 0x0002 ? sizeof(DARC_FILEHEAD_VER5) : sizeof(DARC_FILEHEAD_VER1);
		File = (DARC_FILEHEAD_VER5*)(FileP + Dir->FileHeadAddress);
		for (i = 0; i < Dir->FileHeadNum; i++, File = (DARC_FILEHEAD_VER5*)((u8*)File + FileHeadSize))
		{
			// �ǥ��쥯�ȥ꤫�ɤ����ǄI�����
			if (File->Attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// �ǥ��쥯�ȥ�Έ��Ϥ��َ��򤫤���
				DirectoryDecode(NameP, DirP, FileP, Head, (DARC_DIRECTORY_VER5*)(DirP + File->DataAddress), ArcP, Key, DirPath);
			}
			else
			{
				// Do not override existing files
				Path filePath((DirPath / GetOriginalFileName(NameP + File->NameAddress)));
				if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
					continue;

				// �ǩ`����������ϤΤ�ܞ��
				if (File->DataSize != 0)
				{
					// ����λ�ä򥻥åȤ���
					if (ArcP.tellg() != (Head->DataStartAddress + File->DataAddress))
						ArcP.seekg(Head->DataStartAddress + File->DataAddress);

					// �ǩ`�����R�s����Ƥ��뤫�ɤ����ǄI�����
					if (Head->Version >= 0x0002 && File->CompressedDataSize != 0xffffffff)
					{
						void* temp;

						// �R�s����Ƥ������

						// �R�s�ǩ`�������ޤ�����I��δ_��
						temp = malloc(File->CompressedDataSize);

						// �R�s�ǩ`�����i���z��
						if (Head->Version >= 0x0005)
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key, File->DataSize);
						}
						else
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key);
						}

						// ���
						auto buffer = std::shared_ptr<uint8_t>(new uint8_t[File->DataSize]);
						Decompress(temp, &*buffer);

						// ����ν��
						free(temp);

						std::ofstream ofs((DirPath / GetOriginalFileName(NameP + File->NameAddress)), std::ios_base::binary);
						ofs.write((const char*)&*buffer, File->DataSize);

					}
					else
					{
						// �R�s����Ƥ��ʤ�����

						// ܞ�̈́I���_ʼ
						{
							u32 MoveSize, WriteSize;

							WriteSize = 0;

							auto buffer = std::shared_ptr<uint8_t>(new uint8_t[DXA_BUFFERSIZE_VER5]);

							while (WriteSize < File->DataSize)
							{
								MoveSize = File->DataSize - WriteSize > DXA_BUFFERSIZE_VER5 ? DXA_BUFFERSIZE_VER5 : File->DataSize - WriteSize;

								// �ե�����η�ܞ�i���z��
								if (Head->Version >= 0x0005)
								{
									KeyConvFileRead(&*buffer + WriteSize, MoveSize, ArcP, Key, File->DataSize + WriteSize);
								}
								else
								{
									KeyConvFileRead(&*buffer + WriteSize, MoveSize, ArcP, Key);
								}

								WriteSize += MoveSize;
							}

							std::ofstream ofs((DirPath / GetOriginalFileName(NameP + File->NameAddress)), std::ios_base::binary);
							ofs.write((const char*)&*buffer, File->DataSize);
						}
					}
				}
			}
		}
	}

	// �K��
	return 0;
}

int DecodeArchive(Path& path, DXArchive* output = NULL)
{
	u8* HeadBuffer = NULL;
	DARC_HEAD_VER5 Head;
	char OldDir[MAX_PATH];
	u8 Key[DXA_KEYSTR_LENGTH_VER5];

	// �I�����Ф�����
	KeyCreate(NULL, Key);

	// ���`�����֥ե�������_��
	std::string ps(path.string());
	const char* ArchiveName = ps.c_str();
	std::ifstream ArcP(ArchiveName, std::ios_base::binary);
	if (!ArcP.is_open()) return {};

	// �إå����������
	{
		KeyConvFileRead(&Head, sizeof(DARC_HEAD_VER5), ArcP, Key, 0);

		// �ɣĤΗʖ�
		if (Head.Head != DXA_HEAD_VER5)
		{
			// �Щ`�������ǰ���{�٤�
			memset(Key, 0xffffffff, DXA_KEYSTR_LENGTH_VER5);

			ArcP.seekg(0);
			KeyConvFileRead(&Head, sizeof(DARC_HEAD_VER5), ArcP, Key, 0);

			// �Щ`�������ǰ�Ǥ�ʤ����Ϥϥ���`
			if (Head.Head != DXA_HEAD_VER5)
				return -1;
		}

		// �Щ`�����ʖ�
		if (Head.Version > DXA_VER_VER5)
			return -1;

		// �إå��Υ������֤Υ����_������
		std::vector<dxa::u8> HeadBuffer(Head.HeadSize);

		// �إå��ѥå��������i���z��
		ArcP.seekg(Head.FileNameTableStartAddress);
		if (Head.Version >= 0x0005)
		{
			KeyConvFileRead(HeadBuffer.data(), Head.HeadSize, ArcP, Key, 0);
		}
		else
		{
			KeyConvFileRead(HeadBuffer.data(), Head.HeadSize, ArcP, Key);
		}

		// �����ɥ쥹�򥻥åȤ���
		u8* FileP, * NameP, * DirP;
		NameP = HeadBuffer.data();
		FileP = NameP + Head.FileTableStartAddress;
		DirP = NameP + Head.DirectoryTableStartAddress;

		// ���`�����֤�չ�_���_ʼ����
		if (output != NULL)
			DirectoryDecode(NameP, DirP, FileP, &Head, (DARC_DIRECTORY_VER5*)DirP, ArcP, Key, ".", *output);
		else
			DirectoryDecode(NameP, DirP, FileP, &Head, (DARC_DIRECTORY_VER5*)DirP, ArcP, Key, (path.parent_path() / path.stem()).string());
	}

	// �K��
	return 0;
}

}

DXArchive extractDxaToMem(const StringPath& path)
{
	Path p(path);
	if (!std::filesystem::is_regular_file(p)) return {};

	DXArchive a;
	dxa::DecodeArchive(p, &a);

	return a;
}

int extractDxaToFile(const StringPath& path)
{
	Path p(path);
	if (!std::filesystem::is_regular_file(p)) return {};

	DXArchive a;
	dxa::DecodeArchive(p);

	return 0;
}
