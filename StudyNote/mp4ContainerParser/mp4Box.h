#ifndef MP4BOX_H
#define MP4BOX_H

#include"bitStream.h"
#include"mp4BoxDefine.h"

#define SAFE_DELETE(x) {if(x){delete (x);(x)=NULL;}}
#define SAFE_DELETE_ARRAY(x) {if(x){delete[] (x);(x)=NULL;}}

class Mp4Box
{
public:
	Mp4Box(wchar_t* parserFilePath);
	Mp4Box(wchar_t* parserFilePath, wchar_t* outputInfoFilePath);
	~Mp4Box();
	MP4ERROR StartGenerateMp4BoxTree();
	MP4ERROR StartParseBoxTreeData();
	MP4ERROR StartPrintBoxTreeInfo();
private:
	//Generate box tree
	mp4_box_t* GenerateBoxTree();
	mp4_box_t* FindSonBoxFromBuffer(mp4_box_t* fatherBox, Int64 sonBoxStartPos);
	mp4_box_t* FindNextBrotherBoxFromBuffer(mp4_box_t* fatherBox, mp4_box_t* frontBrotherBox);
	void CreateUUID(uuid_t* p_uuid, UInt32 i_fourcc);
	void SetBoxSize(mp4_box_t* BoxNode);
	void SetUUID(mp4_box_t* BoxNode);
	UInt8 CaclBoxHeaderSize(mp4_box_t* BoxNode);

	//parse box data
	void CoreParseBoxTreeData(mp4_box_t* boxNode);
	void ParseBoxData(mp4_box_t* boxNode);
	void Parse_ATOM_ftyp(mp4_box_t* boxNode);
	void Parse_ATOM_mvhd(mp4_box_t* boxNode);
	void Parse_ATOM_tkhd(mp4_box_t* boxNode);
	void Parse_ATOM_mdhd(mp4_box_t* boxNode);
	void Parse_ATOM_hdlr(mp4_box_t* boxNode);
	void Parse_ATOM_vmhd(mp4_box_t* boxNode);
	void Parse_ATOM_smhd(mp4_box_t* boxNode);
	void Parse_ATOM_hmhd(mp4_box_t* boxNode);
	void Parse_ATOM_dref(mp4_box_t* boxNode);
	void Parse_ATOM_URL(mp4_box_t* boxNode);
	void Parse_ATOM_URN(mp4_box_t* boxNode);
	void Parse_ATOM_stsd(mp4_box_t* boxNode);
	void Parse_ATOM_stts(mp4_box_t* boxNode);
	void Parse_ATOM_stss(mp4_box_t* boxNode);
	void Parse_ATOM_ctts(mp4_box_t* boxNode);
	void Parse_ATOM_stsc(mp4_box_t* boxNode);
	void Parse_ATOM_stsz(mp4_box_t* boxNode);
	void Parse_ATOM_stz2(mp4_box_t* boxNode);
	void Parse_ATOM_stco(mp4_box_t* boxNode);
	void Parse_ATOM_co64(mp4_box_t* boxNode);

	//Print box Tree info to info.txt
	void RecursivePrintBoxTree(mp4_box_t* boxNode);
	void PrintBoxNode(mp4_box_t* boxNode, mp4_box_t* boxSonNode);
	void WriteCurSymbol(bool isLeaf, char* printString, mp4_box_t* boxNode);
	void PrintBoxData(mp4_box_t* boxNode);
	void Print_ATOM_ftyp_info(mp4_box_t* boxNode);
	void Print_ATOM_mvhd_info(mp4_box_t* boxNode);
	void Print_ATOM_tkhd_info(mp4_box_t* boxNode);
	void Print_ATOM_mdhd_info(mp4_box_t* boxNode);
	void Print_ATOM_hdlr_info(mp4_box_t* boxNode);
	void Print_ATOM_vmhd_info(mp4_box_t* boxNode);
	void Print_ATOM_smhd_info(mp4_box_t* boxNode);
	void Print_ATOM_hmhd_info(mp4_box_t* boxNode);
	void Print_ATOM_dref_info(mp4_box_t* boxNode);
	void Print_ATOM_URL_info(mp4_box_t* boxNode);
	void Print_ATOM_URN_info(mp4_box_t* boxNode);
	void Print_ATOM_stsd_info(mp4_box_t* boxNode);
	void Print_ATOM_stts_info(mp4_box_t* boxNode);
	void Print_ATOM_stss_info(mp4_box_t* boxNode);
	void Print_ATOM_ctts_info(mp4_box_t* boxNode);
	void Print_ATOM_stsc_info(mp4_box_t* boxNode);
	void Print_ATOM_stsz_info(mp4_box_t* boxNode);
	void Print_ATOM_stz2_info(mp4_box_t* boxNode);
	void Print_ATOM_stco_info(mp4_box_t* boxNode);
	void Print_ATOM_co64_info(mp4_box_t* boxNode);
	//search box
	mp4_box_t* SearchBox(UInt32 type, const char* UUID);

	//Release tree
	void ReleaseBox(mp4_box_t* rootBox);
	void ReleaseBoxData(mp4_box_t* boxNode);
	void Release_ATOM_ftyp(mp4_box_t* boxNode);
	void Release_ATOM_mvhd(mp4_box_t* boxNode);
	void Release_ATOM_tkhd(mp4_box_t* boxNode);
	void Release_ATOM_mdhd(mp4_box_t* boxNode);
	void Release_ATOM_hdlr(mp4_box_t* boxNode);
	void Release_ATOM_vmhd(mp4_box_t* boxNode);
	void Release_ATOM_smhd(mp4_box_t* boxNode);
	void Release_ATOM_hmhd(mp4_box_t* boxNode);
	void Release_dref_ftyp(mp4_box_t* boxNode);
	void Release_ATOM_URL(mp4_box_t* boxNode);
	void Release_ATOM_URN(mp4_box_t* boxNode);
	void Release_ATOM_stsd(mp4_box_t* boxNode);
	void Release_ATOM_stts(mp4_box_t* boxNode);
	void Release_ATOM_stss(mp4_box_t* boxNode);
	void Release_ATOM_ctts(mp4_box_t* boxNode);
	void Release_ATOM_stsc(mp4_box_t* boxNode);
	void Release_ATOM_stsz(mp4_box_t* boxNode);
	void Release_ATOM_stz2(mp4_box_t* boxNode);
	void Release_ATOM_stco(mp4_box_t* boxNode);
	void Release_ATOM_co64(mp4_box_t* boxNode);
private:
	InBitStream* m_ParserfileStream;
	OutBitStream* m_OutputInfoStream;
	mp4_box_t* m_boxRoot;
};

#endif //MP4BOX_H
