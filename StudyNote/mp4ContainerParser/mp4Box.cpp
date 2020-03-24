#include"mp4Box.h"
#include<stack>

//public fucntion
Mp4Box::Mp4Box(wchar_t* parserFilePath)
{
	m_ParserfileStream = new InBitStream(parserFilePath);
	m_OutputInfoStream = new OutBitStream();
	m_boxRoot = NULL;
}

Mp4Box::Mp4Box(wchar_t* parserFilePath, wchar_t* outputInfoFilePath)
{
	m_ParserfileStream = new InBitStream(parserFilePath);
	m_OutputInfoStream = new OutBitStream(outputInfoFilePath);
	m_boxRoot = NULL;
}

Mp4Box::~Mp4Box()
{
	if (m_ParserfileStream != NULL)
	{
		delete m_ParserfileStream;
		m_ParserfileStream = NULL;
	}

	if (m_OutputInfoStream != NULL)
	{
		delete m_OutputInfoStream;
		m_OutputInfoStream = NULL;
	}

	ReleaseBox(m_boxRoot);
	m_boxRoot = NULL;
}

MP4ERROR Mp4Box::StartGenerateMp4BoxTree()
{
	MP4ERROR error = MP4_OK;
	error = m_ParserfileStream->OpenStream();
	if (error != MP4_OK)
		return error;

	m_boxRoot = GenerateBoxTree();
	if (m_boxRoot == NULL)
		return MP4_BOX_TREE_GEN_FAIL;

	return error;
}

MP4ERROR Mp4Box::StartParseBoxTreeData()
{
	if (m_boxRoot == NULL)
		return ROOT_NULL;

	CoreParseBoxTreeData(m_boxRoot);
	return MP4_OK;
}

MP4ERROR Mp4Box::StartPrintBoxTreeInfo()
{
	MP4ERROR error = MP4_OK;
	error = m_OutputInfoStream->OpenStream();
	if (error != MP4_OK)
		return error;

	RecursivePrintBoxTree(m_boxRoot);

	return MP4_OK;
}

////Generate box tree
mp4_box_t* Mp4Box::GenerateBoxTree()
{
	mp4_box_t* boxRoot = new mp4_box_t;
	boxRoot->i_pos = 0;
	boxRoot->i_type = ATOM_root;
	boxRoot->i_shortsize = 1;
	boxRoot->i_size = m_ParserfileStream->GetFileSize();
	CreateUUID(&boxRoot->i_uuid, boxRoot->i_type);
	boxRoot->data.p_data = NULL;
	boxRoot->p_father = NULL;
	boxRoot->p_son = NULL;
	boxRoot->p_frontBrother = NULL;
	boxRoot->p_nextBrother = NULL;

	//root box no brother
	boxRoot->p_son = FindSonBoxFromBuffer(boxRoot, 0);

	return boxRoot;
}

mp4_box_t* Mp4Box::FindSonBoxFromBuffer(mp4_box_t* fatherBox, Int64 sonBoxStartPos)
{
	if (NULL == fatherBox)
		return NULL;

	if (m_ParserfileStream->Seek(sonBoxStartPos) != MP4_OK)
		return NULL;

	//at least have size(4)+type(4) = 8 byte
	if (m_ParserfileStream->Peek(8) != MP4_OK)
		return NULL;

	mp4_box_t* firstSonbox = new mp4_box_t;
	firstSonbox->data.p_data = NULL;
	firstSonbox->p_father = fatherBox;
	firstSonbox->p_son = NULL;
	firstSonbox->p_frontBrother = NULL;
	firstSonbox->p_nextBrother = NULL;
	firstSonbox->i_pos = sonBoxStartPos;

	m_ParserfileStream->Get4ByteOrder(&firstSonbox->i_shortsize);
	m_ParserfileStream->Get4ByteOrder(&firstSonbox->i_type);

	//get father header size 
	Int32 fatherHeaderSize = CaclBoxHeaderSize(fatherBox);

	//get box size
	SetBoxSize(firstSonbox);

	//get uuid or gen uuid
	SetUUID(firstSonbox);

	UInt64 sonBoxAndFatherHeaderSize = firstSonbox->i_size + fatherHeaderSize;
	mp4_box_t* curBrotherBox = firstSonbox;
	//search all brother box
	while (sonBoxAndFatherHeaderSize < fatherBox->i_size)
	{
		curBrotherBox->p_nextBrother = FindNextBrotherBoxFromBuffer(fatherBox, curBrotherBox);
		if (NULL == curBrotherBox->p_nextBrother)
			break;
		curBrotherBox = curBrotherBox->p_nextBrother;
		sonBoxAndFatherHeaderSize += curBrotherBox->i_size;
	}
	// allSonBoxSize + fatherHeaderSize = fathersize
	if (sonBoxAndFatherHeaderSize != fatherBox->i_size)
	{
		ReleaseBox(firstSonbox);
		firstSonbox = NULL;
	}
	else //find brother`s son
	{
		curBrotherBox = firstSonbox;
		while (curBrotherBox)
		{
			Int64 firstSonPos = curBrotherBox->i_pos + CaclBoxHeaderSize(curBrotherBox);

			if (curBrotherBox->i_type != ATOM_uuid)
			{
				curBrotherBox->p_son = FindSonBoxFromBuffer(curBrotherBox, firstSonPos);
			}

			curBrotherBox = curBrotherBox->p_nextBrother;
		}
	}
	return firstSonbox;
}

mp4_box_t* Mp4Box::FindNextBrotherBoxFromBuffer(mp4_box_t* fatherBox, mp4_box_t* frontBrotherBox)
{
	if (NULL == fatherBox || NULL == frontBrotherBox)
		return NULL;

	if (m_ParserfileStream->Seek(frontBrotherBox->i_pos + frontBrotherBox->i_size) != MP4_OK)
		return NULL;

	if (m_ParserfileStream->Peek(8) != MP4_OK)
		return NULL;

	mp4_box_t* tempBrotherbox = new mp4_box_t;
	tempBrotherbox->data.p_data = NULL;
	tempBrotherbox->p_father = fatherBox;
	tempBrotherbox->p_son = NULL;
	tempBrotherbox->p_frontBrother = frontBrotherBox;
	tempBrotherbox->p_nextBrother = NULL;
	tempBrotherbox->i_pos = m_ParserfileStream->Tell();

	m_ParserfileStream->Get4ByteOrder(&tempBrotherbox->i_shortsize);
	m_ParserfileStream->Get4ByteOrder(&tempBrotherbox->i_type);

	SetBoxSize(tempBrotherbox);

	SetUUID(tempBrotherbox);

	return tempBrotherbox;
}

void Mp4Box::CreateUUID(uuid_t* p_uuid, UInt32 i_fourcc)
{
	/* made by 0xXXXXXXXX-0011-0010-8000-00aa00389b71 where XXXXXXXX is the fourcc */
	*((UInt32*)p_uuid) = i_fourcc;
	*(((UInt32*)p_uuid) + 1) = 0x00110010;
	*(((UInt32*)p_uuid) + 2) = 0x800000aa;
	*(((UInt32*)p_uuid) + 3) = 0x00389b71;
}

void Mp4Box::SetUUID(mp4_box_t* BoxNode)
{
	if (BoxNode->i_type == ATOM_uuid)
	{
		m_ParserfileStream->Get16Byte(&BoxNode->i_uuid);
	}
	else
	{
		CreateUUID(&BoxNode->i_uuid, BoxNode->i_type);
	}
}

void Mp4Box::SetBoxSize(mp4_box_t* BoxNode)
{
	if (NULL == BoxNode)
		return;

	//when short size = 1,meaning 32bit is not enough storage box size
	//we should get large size
	//when short size = 0,meaning mdat box in the end
	//size = file end pos - cur pos
	if (BoxNode->i_shortsize == 1)
	{
		m_ParserfileStream->Get8ByteOrder(&BoxNode->i_size);
	}
	else if (BoxNode->i_shortsize == 0)
	{
		BoxNode->i_shortsize = 0;
		if (BoxNode->i_type == ATOM_mdat)
			BoxNode->i_size = m_ParserfileStream->GetFileSize() - BoxNode->i_pos;
	}
	else
	{
		BoxNode->i_size = BoxNode->i_shortsize;
	}
}

UInt8 Mp4Box::CaclBoxHeaderSize(mp4_box_t* BoxNode)
{
	UInt8 headerSize = 0;
	if (BoxNode->i_shortsize == 1)
	{
		headerSize = 16; //short size(4) + type(4) + large size(8)
	}
	else
	{
		headerSize = 8; //short size(4) + type(4)
	}

	if (BoxNode->i_type == ATOM_root)
		headerSize = 0;

	switch (BoxNode->i_type)
	{
	case ATOM_root:
		headerSize = 0;
		break;
	case ATOM_dref:  //version int(8) + flag bit(24) + data count int(32)
	case ATOM_stsd:
		headerSize += 8;
		break;
	}

	return headerSize;
}

////parse box data
void Mp4Box::CoreParseBoxTreeData(mp4_box_t* boxNode)
{
	if (NULL == boxNode)
		return;

	if (boxNode->p_son == NULL && boxNode->p_nextBrother == NULL)
	{
		ParseBoxData(boxNode);
		return;
	}

	CoreParseBoxTreeData(boxNode->p_son);
	CoreParseBoxTreeData(boxNode->p_nextBrother);

	ParseBoxData(boxNode);
}

void Mp4Box::ParseBoxData(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
	m_ParserfileStream->Seek(CaclBoxHeaderSize(boxNode) + boxNode->i_pos);

	switch (boxNode->i_type)
	{
	case ATOM_ftyp:
		Parse_ATOM_ftyp(boxNode);
		break;
	case ATOM_mvhd:
		Parse_ATOM_mvhd(boxNode);
		break;
	case ATOM_tkhd:
		Parse_ATOM_tkhd(boxNode);
		break;
	case ATOM_mdhd:
		Parse_ATOM_mdhd(boxNode);
		break;
	case ATOM_hdlr:
		Parse_ATOM_hdlr(boxNode);
		break;
	case ATOM_vmhd:
		Parse_ATOM_vmhd(boxNode);
		break;
	case ATOM_smhd:
		Parse_ATOM_smhd(boxNode);
		break;
	case ATOM_hmhd:
		Parse_ATOM_hmhd(boxNode);
		break;
	case ATOM_dref:
		Parse_ATOM_dref(boxNode);
		break;
	case ATOM_url:
		Parse_ATOM_URL(boxNode);
		break;
	case ATOM_urn:
		Parse_ATOM_URN(boxNode);
		break;
	case ATOM_stts:
		Parse_ATOM_stts(boxNode);
		break;
	case ATOM_stss:
		Parse_ATOM_stss(boxNode);
		break;
	case ATOM_ctts:
		Parse_ATOM_ctts(boxNode);
		break;
	case ATOM_stsc:
		Parse_ATOM_stsc(boxNode);
		break;
	case ATOM_stsz:
		Parse_ATOM_stsz(boxNode);
		break;
	case ATOM_stz2:
		Parse_ATOM_stz2(boxNode);
		break;
	case ATOM_stco:
		Parse_ATOM_stco(boxNode);
		break;
	case ATOM_co64:
		Parse_ATOM_co64(boxNode);
		break;
	case ATOM_stsd:
		Parse_ATOM_stsd(boxNode);
		break;
	default:
		break;
	}
}

void Mp4Box::Parse_ATOM_ftyp(mp4_box_t* boxNode)
{
	boxNode->data.p_ftyp = new mp4_box_data_ftyp_t;
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_ftyp->major_brand);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_ftyp->minor_version);
	boxNode->data.p_ftyp->compatible_brands_count = (boxNode->i_size - 16) / 4;
	boxNode->data.p_ftyp->compatible_brands = new UInt32[boxNode->data.p_ftyp->compatible_brands_count];
	for (int i = 0; i < boxNode->data.p_ftyp->compatible_brands_count; ++i)
	{
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_ftyp->compatible_brands[i]);
	}
}

void Mp4Box::Parse_ATOM_mvhd(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_mvhd = new mp4_box_data_mvhd_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_mvhd->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_mvhd->flags);
	if (boxNode->data.p_mvhd->version == 1)
	{
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_mvhd->creation_time);
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_mvhd->modification_time);
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_mvhd->timescale);
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_mvhd->duration);
	}
	else
	{
		UInt32 temp;
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_mvhd->creation_time = temp;
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_mvhd->modification_time = temp;
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_mvhd->timescale);
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_mvhd->duration = temp;
	}
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_mvhd->rate[0]);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_mvhd->rate[1]);
	m_ParserfileStream->GetByte(&boxNode->data.p_mvhd->volume[0]);
	m_ParserfileStream->GetByte(&boxNode->data.p_mvhd->volume[1]);
	//bit(16) + uint(32)[2] + int(32)[9] + bit[32][6] = 70 (byte)
	m_ParserfileStream->Seek(70, CUR); //reserved
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_mvhd->next_track_id);
}

void Mp4Box::Parse_ATOM_tkhd(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_tkhd = new mp4_box_data_tkhd_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_tkhd->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_tkhd->flags);
	if (boxNode->data.p_tkhd->version == 1)
	{
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_tkhd->creation_time);
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_tkhd->modification_time);
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_tkhd->track_id);
		m_ParserfileStream->Seek(4, CUR); //reserved
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_tkhd->duration);
	}
	else
	{
		UInt32 temp;
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_tkhd->creation_time = temp;
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_tkhd->modification_time = temp;
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_tkhd->track_id);
		m_ParserfileStream->Seek(4, CUR); //reserved
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_tkhd->duration = temp;
	}
	m_ParserfileStream->Seek(8, CUR); //reserved
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_tkhd->layer);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_tkhd->alternate_group);
	m_ParserfileStream->GetByte(&boxNode->data.p_tkhd->volume[0]);  //auido 0x01 00 other 0
	m_ParserfileStream->GetByte(&boxNode->data.p_tkhd->volume[1]);
	//int(16) + int(32)[9] = 38 (byte)
	m_ParserfileStream->Seek(38, CUR);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_tkhd->width[0]);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_tkhd->width[1]);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_tkhd->height[0]);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_tkhd->height[1]);
}

void Mp4Box::Parse_ATOM_mdhd(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_mdhd = new mp4_box_data_mdhd_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_mdhd->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_mdhd->flags);
	if (boxNode->data.p_mdhd->version == 1)
	{
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_mdhd->creation_time);
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_mdhd->modification_time);
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_mdhd->timescale);
		m_ParserfileStream->Get8ByteOrder(&boxNode->data.p_mdhd->duration);
	}
	else
	{
		UInt32 temp;
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_mdhd->creation_time = temp;
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_mdhd->modification_time = temp;
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_mdhd->timescale);
		m_ParserfileStream->Get4ByteOrder(&temp);
		boxNode->data.p_mdhd->duration = temp;
	}
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_mdhd->language);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_mdhd->predefined);
}

void Mp4Box::Parse_ATOM_hdlr(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_hdlr = new mp4_box_data_hdlr_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_hdlr->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_hdlr->flags);

	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_hdlr->predefined);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_hdlr->handler_type);
	//int(32)[3] reserved
	//string name

}

void Mp4Box::Parse_ATOM_vmhd(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_vmhd = new mp4_box_data_vmhd_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_vmhd->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_vmhd->flags);

	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_vmhd->graphics_mode);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_vmhd->opcolor[0]);//r
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_vmhd->opcolor[1]);//g
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_vmhd->opcolor[2]);//b
}

void Mp4Box::Parse_ATOM_smhd(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_smhd = new mp4_box_data_smhd_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_smhd->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_smhd->flags);

	m_ParserfileStream->GetByte(&boxNode->data.p_smhd->balance[0]);
	m_ParserfileStream->GetByte(&boxNode->data.p_smhd->balance[1]);
	//reserved int[16]
}

void Mp4Box::Parse_ATOM_hmhd(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_hmhd = new mp4_box_data_hmhd_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_hmhd->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_hmhd->flags);

	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_hmhd->max_PDU_size);
	m_ParserfileStream->Get2ByteOrder(&boxNode->data.p_hmhd->avg_PDU_size);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_hmhd->max_bitrate);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_hmhd->avg_bitrate);
	//UInt32 reserved;
}

void Mp4Box::Parse_ATOM_dref(mp4_box_t* boxNode)
{
	//full box and container
	boxNode->data.p_dref = new mp4_box_data_dref_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_dref->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_dref->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_dref->entry_count);
}

void Mp4Box::Parse_ATOM_URL(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_url = new mp4_box_data_url_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_url->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_url->flags);
	//string location
}

void Mp4Box::Parse_ATOM_URN(mp4_box_t* boxNode)
{
	//full box
	boxNode->data.p_urn = new mp4_box_data_urn_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_urn->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_urn->flags);
	//string name
	//string location
}

void Mp4Box::Parse_ATOM_stts(mp4_box_t* boxNode)
{
	//full box
	// sample_count[1] * sample_delta[1] + sample_count[2] * sample_delta[2] + ....... = duration(time scale)

	boxNode->data.p_stts = new mp4_box_data_stts_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_stts->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_stts->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stts->entry_count);

	boxNode->data.p_stts->sample_count = new UInt32[boxNode->data.p_stts->entry_count];
	boxNode->data.p_stts->sample_delta = new Int32[boxNode->data.p_stts->entry_count];

	for (int i = 0; i < boxNode->data.p_stts->entry_count; ++i)
	{
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stts->sample_count[i]);
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stts->sample_delta[i]);
	}

}

void Mp4Box::Parse_ATOM_stss(mp4_box_t* boxNode)
{
	//full box
	//Sync Sample Box 
	//keyframe list (access point list)
	//If this box is not present, every sample is a random access point. 
	boxNode->data.p_stss = new mp4_box_data_stss_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_stss->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_stss->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stss->entry_count);

	boxNode->data.p_stss->sample_number = new UInt32[boxNode->data.p_stss->entry_count];

	for (int i = 0; i < boxNode->data.p_stss->entry_count; ++i)
	{
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stss->sample_number[i]);
	}

}

void Mp4Box::Parse_ATOM_ctts(mp4_box_t* boxNode)
{
	//full box
	//	DT(decoding time)
	//	CT(composition time)
	//	Decode Delta(stts)
	//	Composition offset = CT - DT
	boxNode->data.p_ctts = new mp4_box_data_ctts_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_ctts->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_ctts->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_ctts->entry_count);

	boxNode->data.p_ctts->sample_count = new UInt32[boxNode->data.p_ctts->entry_count];
	boxNode->data.p_ctts->sample_offset = new Int32[boxNode->data.p_ctts->entry_count];

	for (int i = 0; i < boxNode->data.p_ctts->entry_count; ++i)
	{
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_ctts->sample_count[i]);
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_ctts->sample_offset[i]);
	}

}

void Mp4Box::Parse_ATOM_stsc(mp4_box_t* boxNode)
{
	//full box
	//first_chunk /samples_per_chunk /sample_description_index
	boxNode->data.p_stsc = new mp4_box_data_stsc_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_stsc->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_stsc->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stsc->entry_count);

	boxNode->data.p_stsc->first_chunk = new UInt32[boxNode->data.p_stsc->entry_count];
	boxNode->data.p_stsc->samples_per_chunk = new UInt32[boxNode->data.p_stsc->entry_count];
	boxNode->data.p_stsc->sample_description_index = new UInt32[boxNode->data.p_stsc->entry_count];

	for (int i = 0; i < boxNode->data.p_stsc->entry_count; ++i)
	{
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stsc->first_chunk[i]);
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stsc->samples_per_chunk[i]);
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stsc->sample_description_index[i]);
	}

}

void Mp4Box::Parse_ATOM_stsz(mp4_box_t* boxNode)
{
	//full box
	//sample count
	//each sample size
	boxNode->data.p_stsz = new mp4_box_data_stsz_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_stsz->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_stsz->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stsz->sample_size);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stsz->sample_count);

	boxNode->data.p_stsz->entry_size = new UInt32[boxNode->data.p_stsz->sample_count];
	if (boxNode->data.p_stsz->sample_size == 0)
	{
		for (int i = 0; i < boxNode->data.p_stsz->sample_count; ++i)
		{
			m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stsz->entry_size[i]);
		}
	}
}

void Mp4Box::Parse_ATOM_stz2(mp4_box_t* boxNode)
{
	//full box
	//sample count
	//each sample size
	boxNode->data.p_stz2 = new mp4_box_data_stz2_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_stz2->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_stz2->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stz2->sample_size);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stz2->sample_count);

	boxNode->data.p_stz2->entry_size = new UInt32[boxNode->data.p_stz2->sample_count];
	if (boxNode->data.p_stz2->sample_size == 0)
	{
		for (int i = 0; i < boxNode->data.p_stz2->sample_count; ++i)
		{
			m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stz2->entry_size[i]);
		}
	}
}

void Mp4Box::Parse_ATOM_stco(mp4_box_t* boxNode)
{
	//full box
	//trunk position offset in mdat
	boxNode->data.p_stco = new mp4_box_data_stco_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_stco->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_stco->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stco->entry_count);

	boxNode->data.p_stco->chunk_offset = new UInt32[boxNode->data.p_stco->entry_count];

	for (int i = 0; i < boxNode->data.p_stco->entry_count; ++i)
	{
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_stco->chunk_offset[i]);
	}

}

void Mp4Box::Parse_ATOM_co64(mp4_box_t* boxNode)
{
	//full box
	//trunk position offset in mdat
	boxNode->data.p_co64 = new mp4_box_data_co64_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_co64->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_co64->flags);
	m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_co64->entry_count);

	boxNode->data.p_co64->chunk_offset = new UInt64[boxNode->data.p_co64->entry_count];

	for (int i = 0; i < boxNode->data.p_co64->entry_count; ++i)
	{
		m_ParserfileStream->Get4ByteOrder(&boxNode->data.p_co64->chunk_offset[i]);
	}

}

void Mp4Box::Parse_ATOM_stsd(mp4_box_t* boxNode)
{
	//full box
	//Sample Description Box
	boxNode->data.p_stsd = new mp4_box_data_stsd_t;
	m_ParserfileStream->GetByte(&boxNode->data.p_stsd->version);
	m_ParserfileStream->Get3Byte(&boxNode->data.p_stsd->flags);

	//lass SampleDescriptionBox(unsigned int(32) handler_type)
	//	extends FullBox('stsd', 0, 0){
	//		int i;
	//		unsigned int(32) entry_count;
	//		for (i = 1; i <= entry_count; i++){
	//			switch (handler_type){
	//			case ‘soun’: // for audio tracks
	//				AudioSampleEntry();
	//				break;
	//			case ‘vide’: // for video tracks
	//				VisualSampleEntry();
	//				break;
	//			case ‘hint’: // Hint track
	//				HintSampleEntry();
	//				break;
	//			case ‘meta’: // Metadata track
	//				MetadataSampleEntry();
	//				break;
	//			}
	//		}
	//	}

}

////Print box Tree info to info.txt
void Mp4Box::RecursivePrintBoxTree(mp4_box_t* boxNode)
{
	if (NULL == boxNode)
		return;
	//m_ouputInfoStream->WriteBox(boxNode->i_type);
	PrintBoxNode(boxNode, NULL);

	if (boxNode->p_son)
		RecursivePrintBoxTree(boxNode->p_son);

	if (boxNode->p_nextBrother)
		RecursivePrintBoxTree(boxNode->p_nextBrother);
}

void Mp4Box::PrintBoxNode(mp4_box_t* boxNode, mp4_box_t* boxSonNode)
{
	if (boxNode->p_father)
		PrintBoxNode(boxNode->p_father, boxNode);

	if (NULL == boxSonNode) //leaf node
	{
		if (boxNode->p_nextBrother) 
		{
			WriteCurSymbol(true, "|---", boxNode);
		}
		else
		{
			if (boxNode->p_father)
				WriteCurSymbol(true, "`---", boxNode);
			else //root
				WriteCurSymbol(true, "", boxNode);
		}
	}
	else //non leaf node
	{
		if (boxNode->p_nextBrother)
			WriteCurSymbol(false, "|   ", 0);
		else
		{
			if (boxNode->p_father)
				WriteCurSymbol(false, "    ", boxNode);
			else //root
				WriteCurSymbol(false, "", boxNode);
		}
	}
}

void Mp4Box::WriteCurSymbol(bool isLeaf, char* printString, mp4_box_t* boxNode)
{
	m_OutputInfoStream->WriteString(printString);
	if (isLeaf)
	{
		m_OutputInfoStream->WriteUInt32Char2Stream(boxNode->i_type);
		PrintBoxData(boxNode);
		m_OutputInfoStream->WriteChar('\n');
		m_OutputInfoStream->FlushStream();
	}
}

void Mp4Box::PrintBoxData(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	switch (boxNode->i_type)
	{
	case ATOM_ftyp:
		Print_ATOM_ftyp_info(boxNode);
		break;
	case ATOM_mvhd:
		Print_ATOM_mvhd_info(boxNode);
		break;
	case ATOM_tkhd:
		Print_ATOM_tkhd_info(boxNode);
		break;
	case ATOM_mdhd:
		Print_ATOM_mdhd_info(boxNode);
		break;
	case ATOM_hdlr:
		Print_ATOM_hdlr_info(boxNode);
		break;
	case ATOM_vmhd:
		Print_ATOM_vmhd_info(boxNode);
		break;
	case ATOM_smhd:
		Print_ATOM_smhd_info(boxNode);
		break;
	case ATOM_hmhd:
		Print_ATOM_hmhd_info(boxNode);
		break;
	case ATOM_url:
		Print_ATOM_URL_info(boxNode);
		break;
	case ATOM_urn:
		Print_ATOM_URN_info(boxNode);
		break;
	case ATOM_stts:
		Print_ATOM_stts_info(boxNode);
		break;
	case ATOM_stss:
		Print_ATOM_stss_info(boxNode);
		break;
	case ATOM_ctts:
		Print_ATOM_ctts_info(boxNode);
		break;
	case ATOM_stsc:
		Print_ATOM_stsc_info(boxNode);
		break;
	case ATOM_stsz:
		Print_ATOM_stsz_info(boxNode);
		break;
	case ATOM_stz2:
		Print_ATOM_stz2_info(boxNode);
		break;
	case ATOM_stco:
		Print_ATOM_stco_info(boxNode);
		break;
	case ATOM_co64:
		Print_ATOM_co64_info(boxNode);
		break;
	case ATOM_stsd:
		Print_ATOM_stsd_info(boxNode);
		break;
	default:
		break;
	}
}

void Mp4Box::Print_ATOM_ftyp_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	m_OutputInfoStream->WriteString("(codecID:");
	m_OutputInfoStream->WriteUInt32Char2Stream(boxNode->data.p_ftyp->major_brand);
	m_OutputInfoStream->WriteString("/version:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_ftyp->minor_version);
	m_OutputInfoStream->WriteString("/ohter:");
	for (int i = 0; i < boxNode->data.p_ftyp->compatible_brands_count; ++i)
	{
		m_OutputInfoStream->WriteUInt32Char2Stream(boxNode->data.p_ftyp->compatible_brands[i]);
	}
	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_mvhd_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	//1904.1.1.0
	m_OutputInfoStream->WriteString("(Ctime:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mvhd->creation_time);
	m_OutputInfoStream->WriteString("/Mtime:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mvhd->modification_time);
	m_OutputInfoStream->WriteString("/scale:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mvhd->timescale);
	m_OutputInfoStream->WriteString("/duration:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mvhd->duration);
	m_OutputInfoStream->WriteString("/rate:");		//[16,16] 0x0001 0000 1.0
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_mvhd->rate[0]);
	m_OutputInfoStream->WriteString(".");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_mvhd->rate[1]);
	m_OutputInfoStream->WriteString("/volume:");	//[8,8] 0x01 00  1.0
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mvhd->volume[0]);
	m_OutputInfoStream->WriteString(".");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mvhd->volume[1]);
	m_OutputInfoStream->WriteString("/nextTrackID:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mvhd->next_track_id);
	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_tkhd_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	//1904.1.1.0
	m_OutputInfoStream->WriteString("(Ctime:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_tkhd->creation_time);
	m_OutputInfoStream->WriteString("/Mtime:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_tkhd->modification_time);
	m_OutputInfoStream->WriteString("/track_id:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_tkhd->track_id);
	m_OutputInfoStream->WriteString("/duration:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_tkhd->duration);
	m_OutputInfoStream->WriteString("/layer:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->layer);
	m_OutputInfoStream->WriteString("/alternate_group:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->alternate_group);
	m_OutputInfoStream->WriteString("/volume:");	//[8,8] 0x01 00  1.0
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->volume[0]);
	m_OutputInfoStream->WriteString(".");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->volume[1]);
	m_OutputInfoStream->WriteString("/width:");		//[16,16]
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->width[0]);
	m_OutputInfoStream->WriteString(".");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->width[1]);
	m_OutputInfoStream->WriteString("/height:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->height[0]);
	m_OutputInfoStream->WriteString(".");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_tkhd->height[1]);
	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_mdhd_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	//1904.1.1.0
	m_OutputInfoStream->WriteString("(Ctime:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mdhd->creation_time);
	m_OutputInfoStream->WriteString("/Mtime:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mdhd->modification_time);
	m_OutputInfoStream->WriteString("/scale:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mdhd->timescale);
	m_OutputInfoStream->WriteString("/duration:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_mdhd->duration);
	m_OutputInfoStream->WriteString("/language:");		//1bit pad bit[5](3) language
	char tempChar = '0';
	for (int i = 0; i < 3; ++i)
	{
		tempChar = (boxNode->data.p_mdhd->language & (0x7c00 >> (i * 5))) >> ((2-i)*5); //0x7c00 = 0111 1100 0000 0000
		m_OutputInfoStream->WriteChar(tempChar + 0x60);
	}

	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_hdlr_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	m_OutputInfoStream->WriteString("(media type:");
	m_OutputInfoStream->WriteUInt32Char2Stream(boxNode->data.p_hdlr->handler_type);
	/* "vide" "soun" "hint" "odsm" "crsm" "sdsm" "m7sm" "ocsm" "ipsm" "mjsm" */
	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_vmhd_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	m_OutputInfoStream->WriteString("(graphics_mode:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_vmhd->graphics_mode);
	m_OutputInfoStream->WriteString("/r:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_vmhd->opcolor[0]);
	m_OutputInfoStream->WriteString("/g:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_vmhd->opcolor[1]);
	m_OutputInfoStream->WriteString("/b:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_vmhd->opcolor[2]);
	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_smhd_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	m_OutputInfoStream->WriteString("(balance:");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_smhd->balance[0]);
	m_OutputInfoStream->WriteString(".");
	m_OutputInfoStream->WriteIntNum2string(boxNode->data.p_smhd->balance[1]);
	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_hmhd_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	m_OutputInfoStream->WriteString("(max_PDU_size:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_hmhd->max_PDU_size);
	m_OutputInfoStream->WriteString("/avg_PDU_size:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_hmhd->avg_PDU_size);
	m_OutputInfoStream->WriteString("/max_bitrate:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_hmhd->max_bitrate);
	m_OutputInfoStream->WriteString("/avg_bitrate:");
	m_OutputInfoStream->WriteUIntNum2string(boxNode->data.p_hmhd->avg_bitrate);
	m_OutputInfoStream->WriteString(")");
}

void Mp4Box::Print_ATOM_URL_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_URN_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_stts_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_stss_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_ctts_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_stsc_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_stsz_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_stz2_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_stco_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_co64_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}

void Mp4Box::Print_ATOM_stsd_info(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;
}
////search box
mp4_box_t* Mp4Box::SearchBox(UInt32 type, const char* UUID)
{
	std::stack<mp4_box_t *> s;
	mp4_box_t *curBox = m_boxRoot;
	while (curBox != NULL || !s.empty())
	{
		while (curBox != NULL)
		{
			if (type == curBox->i_type)
			{
				if (type == ATOM_uuid)
				{
					//UUID
				}
				else
					return curBox;
			}
			s.push(curBox);
			curBox = curBox->p_son;
		}
		if (!s.empty())
		{
			curBox = s.top();
			s.pop();
			curBox = curBox->p_nextBrother;
		}
	}
	return NULL;
}

////Release tree
void Mp4Box::ReleaseBox(mp4_box_t* rootBox)
{
	if (NULL == rootBox)
		return;

	if (NULL == rootBox->p_son && NULL == rootBox->p_nextBrother)
	{
		ReleaseBoxData(rootBox);
		SAFE_DELETE(rootBox);
		return;
	}

	ReleaseBox(rootBox->p_nextBrother);
	rootBox->p_nextBrother = NULL;
	ReleaseBox(rootBox->p_son);
	rootBox->p_son = NULL;

	ReleaseBoxData(rootBox);
	SAFE_DELETE(rootBox);
}

void Mp4Box::ReleaseBoxData(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	switch (boxNode->i_type)
	{
	case ATOM_ftyp:
		Release_ATOM_ftyp(boxNode);
		break;
	case ATOM_mvhd:
		Release_ATOM_mvhd(boxNode);
		break;
	case ATOM_tkhd:
		Release_ATOM_tkhd(boxNode);
		break;
	case ATOM_mdhd:
		Release_ATOM_mdhd(boxNode);
		break;
	case ATOM_hdlr:
		Release_ATOM_hdlr(boxNode);
		break;
	case ATOM_vmhd:
		Release_ATOM_vmhd(boxNode);
		break;
	case ATOM_smhd:
		Release_ATOM_smhd(boxNode);
		break;
	case ATOM_hmhd:
		Release_ATOM_hmhd(boxNode);
		break;
	case ATOM_url:
		Release_ATOM_URL(boxNode);
		break;
	case ATOM_urn:
		Release_ATOM_URN(boxNode);
		break;
	case ATOM_stts:
		Release_ATOM_stts(boxNode);
		break;
	case ATOM_stss:
		Release_ATOM_stss(boxNode);
		break;
	case ATOM_ctts:
		Release_ATOM_ctts(boxNode);
		break;
	case ATOM_stsc:
		Release_ATOM_stsc(boxNode);
		break;
	case ATOM_stsz:
		Release_ATOM_stsz(boxNode);
		break;
	case ATOM_stz2:
		Release_ATOM_stz2(boxNode);
		break;
	case ATOM_stco:
		Release_ATOM_stco(boxNode);
		break;
	case ATOM_co64:
		Release_ATOM_co64(boxNode);
		break;
	case ATOM_stsd:
		Release_ATOM_stsd(boxNode);
		break;
	default:
		break;
	}
}

void Mp4Box::Release_ATOM_ftyp(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_ftyp->compatible_brands);
	SAFE_DELETE(boxNode->data.p_ftyp);
}

void Mp4Box::Release_ATOM_mvhd(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_mvhd);
}

void Mp4Box::Release_ATOM_tkhd(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_tkhd);
}

void Mp4Box::Release_ATOM_mdhd(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_mdhd);
}

void Mp4Box::Release_ATOM_hdlr(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_hdlr);
}

void Mp4Box::Release_ATOM_vmhd(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_vmhd);
}

void Mp4Box::Release_ATOM_smhd(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_smhd);
}

void Mp4Box::Release_ATOM_hmhd(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_hmhd);
}

void Mp4Box::Release_ATOM_URL(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_url);
}

void Mp4Box::Release_ATOM_URN(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_urn);
}

void Mp4Box::Release_ATOM_stts(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_stts->sample_count);
	SAFE_DELETE_ARRAY(boxNode->data.p_stts->sample_delta);
	SAFE_DELETE(boxNode->data.p_stts);
}

void Mp4Box::Release_ATOM_stss(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_stss->sample_number);
	SAFE_DELETE(boxNode->data.p_stss);
}

void Mp4Box::Release_ATOM_ctts(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_ctts->sample_count);
	SAFE_DELETE_ARRAY(boxNode->data.p_ctts->sample_offset);
	SAFE_DELETE(boxNode->data.p_ctts);
}

void Mp4Box::Release_ATOM_stsc(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_stsc->first_chunk);
	SAFE_DELETE_ARRAY(boxNode->data.p_stsc->samples_per_chunk);
	SAFE_DELETE_ARRAY(boxNode->data.p_stsc->sample_description_index);
	SAFE_DELETE(boxNode->data.p_stsc);
}

void Mp4Box::Release_ATOM_stsz(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_stsz->entry_size);
	SAFE_DELETE(boxNode->data.p_stsz);
}

void Mp4Box::Release_ATOM_stz2(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_stz2->entry_size);
	SAFE_DELETE(boxNode->data.p_stz2);
}

void Mp4Box::Release_ATOM_stco(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_stco->chunk_offset);
	SAFE_DELETE(boxNode->data.p_stco);
}

void Mp4Box::Release_ATOM_co64(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE_ARRAY(boxNode->data.p_co64->chunk_offset);
	SAFE_DELETE(boxNode->data.p_co64);
}

void Mp4Box::Release_ATOM_stsd(mp4_box_t* boxNode)
{
	if (boxNode == NULL)
		return;

	SAFE_DELETE(boxNode->data.p_stsd);
}