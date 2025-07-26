// ChaosBox.h: interface for the CChaosMix class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ItemManager.h"
#include "Protocol.h"
#include "User.h"

#define MAX_TALISMAN_OF_LUCK 10

enum eChaosMixNumber
{
	CHAOS_MIX_CHAOS_ITEM = 1,
	CHAOS_MIX_DEVIL_SQUARE = 2,
	CHAOS_MIX_PLUS_ITEM_LEVEL1 = 3,
	CHAOS_MIX_PLUS_ITEM_LEVEL2 = 4,
	CHAOS_MIX_DINORANT = 5,
	CHAOS_MIX_FRUIT = 6,
	CHAOS_MIX_WING1 = 7,
	CHAOS_MIX_BLOOD_CASTLE = 8,
	CHAOS_MIX_WING2 = 11,
	CHAOS_MIX_PET1 = 13,
	CHAOS_MIX_PET2 = 14,
	CHAOS_MIX_SIEGE_POTION1 = 15,
	CHAOS_MIX_SIEGE_POTION2 = 16,
	CHAOS_MIX_LIFE_STONE = 17,
	CHAOS_MIX_SENIOR = 18,
	CHAOS_MIX_PLUS_ITEM_LEVEL3 = 22,
	CHAOS_MIX_PLUS_ITEM_LEVEL4 = 23,
	CHAOS_MIX_CAPES = 24,
	CHAOS_MIX_PIECE_OF_HORN = 25,
	CHAOS_MIX_BROKEN_HORN = 26,
	CHAOS_MIX_HORN_OF_FENRIR = 27,
	CHAOS_MIX_HORN_OF_FENRIR_UPGRADE = 28,
	CHAOS_MIX_SHIELD_POTION1 = 30,
	CHAOS_MIX_SHIELD_POTION2 = 31,
	CHAOS_MIX_SHIELD_POTION3 = 32,
	CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_PURITY = 33,
	CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_SMELT = 34,
	CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_RESTORE = 35,
	CHAOS_MIX_ITEM_380 = 36,
	CHAOS_MIX_ILLUSION_TEMPLE = 37,
	CHAOS_MIX_FEATHER_OF_CONDOR = 38,
	CHAOS_MIX_WING3 = 39,
	CHAOS_MIX_CHAOS_CARD = 40,
	CHAOS_MIX_CHERRY_BLOSSOM = 41,
	CHAOS_MIX_SOCKET_ITEM_CREATE_SEED = 42,
	CHAOS_MIX_SOCKET_ITEM_CREATE_SEED_SPHERE = 43,
	CHAOS_MIX_SOCKET_ITEM_MOUNT_SEED_SPHERE = 44,
	CHAOS_MIX_SOCKET_ITEM_UN_MOUNT_SEED_SPHERE = 45,
	CHAOS_MIX_IMPERIAL_GUARDIAN = 46,
	CHAOS_MIX_CHEST = 47,
	CHAOS_MIX_SUMMON_SCROLL = 48,
	CHAOS_MIX_PLUS_ITEM_LEVEL5 = 49,
	CHAOS_MIX_PLUS_ITEM_LEVEL6 = 50,
	CHAOS_MIX_LUCKY_ITEM_CREATE = 51,
	CHAOS_MIX_LUCKY_ITEM_REFINE = 52,

	CHAOS_MIX_MONSTER_WING = 56,
	CHAOS_MIX_SOCKET_WEAPON = 57,

	CHAOS_MIX_CONQUEROR_WING =75,
	CHAOS_MIX_DIABLO=76,
	CHAOS_MIX_ANCIENTSOUL=77,

	CHAOS_MIX_BLOODANGEL_ARMOR_FURY = 78,
	CHAOS_MIX_BLOODANGEL_HELM_FURY = 79,
	CHAOS_MIX_BLOODANGEL_PANTS_FURY = 80,
	CHAOS_MIX_BLOODANGEL_BOOTS_FURY = 81,

	CHAOS_MIX_BLOODANGEL_ARMOR_TRANSCENDENCE = 86,
	CHAOS_MIX_BLOODANGEL_HELM_TRANSCENDENCE = 87,
	CHAOS_MIX_BLOODANGEL_PANTS_TRANSCENDENCE = 88,
	CHAOS_MIX_BLOODANGEL_GLOVES_TRANSCENDENCE = 89,

	CHAOS_MIX_BLOODANGEL_ARMOR_FLURRY = 90,
	CHAOS_MIX_BLOODANGEL_HELM_FLURRY = 91,
	CHAOS_MIX_BLOODANGEL_PANTS_FLURRY = 92,
	CHAOS_MIX_BLOODANGEL_GLOVES_FLURRY = 93,

	CHAOS_MIX_BLOODANGEL_ARMOR_EXTREMITY = 98,
	CHAOS_MIX_BLOODANGEL_BOOTS_EXTREMITY = 99,
	CHAOS_MIX_BLOODANGEL_PANTS_EXTREMITY = 100,
	CHAOS_MIX_BLOODANGEL_GLOVES_EXTREMITY = 101,

	CHAOS_MIX_HOLYANGEL_SWORD = 105,
	CHAOS_MIX_HOLYANGEL_MAGICSWORD = 106,
	CHAOS_MIX_HOLYANGEL_CLAWS = 107,

	CHAOS_MIX_BLOODANGEL_ARMOR_CONQUEST = 106,
	CHAOS_MIX_BLOODANGEL_HELM_CONQUEST = 107,
	CHAOS_MIX_BLOODANGEL_PANTS_CONQUEST = 108, //CHAOS_MIX_HOLYANGEL_SCEPTER
	CHAOS_MIX_BLOODANGEL_GLOVES_CONQUEST = 109,  //CHAOS_MIX_HOLYANGEL_LANCE

	CHAOS_MIX_HOLYANGEL_BOW = 110,
	CHAOS_MIX_HOLYANGEL_QUIVER = 111,
	CHAOS_MIX_HOLYANGEL_STAFF = 112,
	CHAOS_MIX_HOLYANGEL_STICK = 113,
	CHAOS_MIX_BLOODANGEL_ARMOR_HONOR = 114,
	CHAOS_MIX_BLOODANGEL_HELM_HONOR = 115,
	CHAOS_MIX_BLOODANGEL_PANTS_HONOR = 116,
	CHAOS_MIX_BLOODANGEL_GLOVES_HONOR = 117,
	CHAOS_MIX_BLOODANGEL_ARMOR_DESTRUCTION = 118,
	CHAOS_MIX_BLOODANGEL_HELM_DESTRUCTION = 119,
	CHAOS_MIX_BLOODANGEL_PANTS_DESTRUCTION = 120,
	CHAOS_MIX_BLOODANGEL_BOOTS_DESTRUCTION = 121,

	CHAOS_MIX_BLOODANGEL_ARMOR_TENACITY = 126,
	CHAOS_MIX_BLOODANGEL_HELM_TENACITY = 127,
	CHAOS_MIX_BLOODANGEL_PANTS_TENACITY = 128,
	CHAOS_MIX_BLOODANGEL_GLOVES_TENACITY = 129,

	CHAOS_ARCHANGEL_OF_HAMMER = 134,
	CHAOS_BLESSED_DIVINE_SWORD_OF_ARCHA = 135,
	CHAOS_MIX_DARK_ANGEL_ITEM_MIN = 136,
	CHAOS_MIX_DARK_ANGEL_ITEM_MAX = 167,
	CHAOS_MIX_DIVINE_ANGEL_ITEM_MIN = 168,
	CHAOS_MIX_DIVINE_ANGEL_ITEM_MAN = 199,
	CHAOS_MIX_DARK_ANGEL_ARMS_MIX = 200,
	CHAOS_MIX_DARK_ANGEL_ARMS_MAX = 207,
	CHAOS_MIX_BLOOD_ITEM_MIN1=208,
	CHAOS_MIX_BLOOD_ITEM_MAX1=239,
	CHAOS_MIX_MAGIC_STONE = 246,
	CHAOS_MIX_BOUND_WINGS1 = 247,
	CHAOS_MIX_BOUND_WINGS2 = 248, //CHAOS_MIX_BLOODANGEL_ARMOR_REDEMPTION
	CHAOS_MIX_BLOODANGEL_HELM_REDEMPTION = 249,
	CHAOS_MIX_GARUDA_FEATHER = 250, //CHAOS_MIX_BLOODANGEL_PANTS_REDEMPTION
	CHAOS_MIX_WING4 = 251, //CHAOS_MIX_BLOODANGEL_GLOVES_REDEMPTION
	CHAOS_MIX_BLOODANGEL_ARMOR_CAPTIVATION = 252,
	CHAOS_MIX_BLOODANGEL_PANTS_CAPTIVATION = 253,
	CHAOS_MIX_BLOODANGEL_GLOVES_CAPTIVATION = 254,
	CHAOS_MIX_BLOODANGEL_BOOTS_CAPTIVATION = 255,


};

//**********************************************//
//************ Client -> GameServer ************//
//**********************************************//
#if(GAMESERVER_UPDATE>=1000)//2017/03/14
struct PMSG_CHAOS_MIX_RECV_MP
{
	PBMSG_HEAD header; // C1:88
	BYTE type;
	BYTE info;
	
};
#endif
struct PMSG_CHAOS_MIX_RECV
{
	PBMSG_HEAD header; // C1:86
	BYTE type;
	BYTE info;
#if(GAMESERVER_UPDATE>=1200)//2017/03/14
	BYTE unk;
#endif
#if(GAMESERVER_UPDATE>=1000)//2017/03/14
	BYTE Count;
#endif
};

//**********************************************//
//************ GameServer -> Client ************//
//**********************************************//
#if(GAMESERVER_UPDATE>=1000)//2017/03/14
struct PMSG_CHAOS_MIX_SEND_MP
{
	PBMSG_HEAD header; // C1:88
	BYTE type;
};
#endif
struct PMSG_CHAOS_MIX_SEND
{
	PBMSG_HEAD header; // C1:86
	BYTE result;
	BYTE ItemInfo[MAX_ITEM_INFO];
	BYTE Pos;
};

//**********************************************//
//**********************************************//
//**********************************************//

class CChaosBox
{
public:
	CChaosBox();
	virtual ~CChaosBox();
	void ChaosBoxInit(LPOBJ lpObj);
	void ChaosBoxItemDown(LPOBJ lpObj,int slot);
	void ChaosBoxItemKeep(LPOBJ lpObj,int slot);
	void ChaosBoxItemSave(LPOBJ lpObj);
	bool GetTalismanOfLuckRate(LPOBJ lpObj,int* rate);
	bool GetElementalTalismanOfLuckRate(LPOBJ lpObj,int* rate);
	void ChaosItemMix(LPOBJ lpObj);
	void DevilSquareMix(LPOBJ lpObj);
	void PlusItemLevelMix(LPOBJ lpObj,int type);
	void DinorantMix(LPOBJ lpObj);
	void FruitMix(LPOBJ lpObj);
	void Wing2Mix(LPOBJ lpObj,int type);
	void BloodCastleMix(LPOBJ lpObj);
	void Wing1Mix(LPOBJ lpObj);
	void PetMix(LPOBJ lpObj,int type);
	void SiegePotionMix(LPOBJ lpObj,int type);
	void LifeStoneMix(LPOBJ lpObj);
	void SeniorMix(LPOBJ lpObj);
	void PieceOfHornMix(LPOBJ lpObj);
	void BrokenHornMix(LPOBJ lpObj);
	void HornOfFenrirMix(LPOBJ lpObj);
	void HornOfFenrirUpgradeMix(LPOBJ lpObj);
	void ShieldPotionMix(LPOBJ lpObj,int type);
	void JewelOfHarmonyItemPurityMix(LPOBJ lpObj);
	void JewelOfHarmonyItemSmeltMix(LPOBJ lpObj);
	void JewelOfHarmonyItemRestoreMix(LPOBJ lpObj);
	void Item380Mix(LPOBJ lpObj);
	void IllusionTempleMix(LPOBJ lpObj);
	void FeatherOfCondorMix(LPOBJ lpObj);
	void Wing3Mix(LPOBJ lpObj);
	void Wing4Mix(LPOBJ lpObj);
	void GarudaFeather(LPOBJ lpObj);
	void ChaosCardMix(LPOBJ lpObj,int type);
	void CherryBlossomMix(LPOBJ lpObj,int type);
	void SocketItemCreateSeedMix(LPOBJ lpObj);
	void SocketItemCreateSeedSphereMix(LPOBJ lpObj);
	void SocketItemMountSeedSphereMix(LPOBJ lpObj,BYTE info);
	void SocketItemUnMountSeedSphereMix(LPOBJ lpObj,BYTE info);
	void ImperialGuardianMix(LPOBJ lpObj);
	void ChestMix(LPOBJ lpObj);
	void SummonScrollMix(LPOBJ lpObj,int type);
	void LuckyItemCreateMix(LPOBJ lpObj);
	void LuckyItemRefineMix(LPOBJ lpObj);
	void MonsterWingMix(LPOBJ lpObj);
	void SocketWeaponMix(LPOBJ lpObj);
	void PentagramMithrilMix(LPOBJ lpObj);
	void PentagramElixirMix(LPOBJ lpObj);
	void PentagramJewelMix(LPOBJ lpObj);
	void PentagramDecompositeMix(LPOBJ lpObj,int type);
	void PentagramJewelUpgradeLevelMix(LPOBJ lpObj,BYTE info);
	void PentagramJewelUpgradeRankMix(LPOBJ lpObj,BYTE info);
	void CGChaosMixRecv(PMSG_CHAOS_MIX_RECV* lpMsg,int aIndex);
	void CGChaosMixCloseRecv(int aIndex);
	void GCChaosBoxSend(LPOBJ lpObj,BYTE type);
	void GCChaosMixSend(int aIndex,BYTE result,CItem* lpItem);
	void DiabloRingMix(LPOBJ lpObj);
	void ConquerorWingMix(LPOBJ lpObj);
	void AncientSoulMix(LPOBJ lpObj);
	void CGChaosMixRecvMP(PMSG_CHAOS_MIX_RECV* lpMsg,int aIndex);
	void DevilSquareMixMP(LPOBJ lpObj,int count);
	void DinorantMixMP(LPOBJ lpObj,int count);
	void FruitMixMP(LPOBJ lpObj,int count);
	void BloodCastleMixMP(LPOBJ lpObj,int count);
	void SiegePotionMixMP(LPOBJ lpObj,int type,int count);
	void LifeStoneMixMP(LPOBJ lpObj,int count);
	void ShieldPotionMixMP(LPOBJ lpObj,int type,int count);
	void IllusionTempleMixMP(LPOBJ lpObj,int count);
	void ChestMixMP(LPOBJ lpObj,int count);
	void JewelOfHarmonyItemSmeltMixMP(LPOBJ lpObj,int count);
	void BloodItemMix(LPOBJ lpObj);
	void DarkAngelMix(LPOBJ lpObj);
	void PentagramMithrilMixSXI(LPOBJ lpObj,int Mithril,int Bless);
	void PentagramJewelUpgradeLevelMixSXI(LPOBJ lpObj,BYTE info,BYTE Element,BYTE Rune,BYTE Talisman,BYTE Amulet);
	void PentagramJewelUpgradeRankMixSXI(LPOBJ lpObj,BYTE info,BYTE Element1,BYTE Element2);
	void PentagramEcstasySlotMix(LPOBJ lpObj,BYTE Debris1,BYTE Debris2,BYTE Debris3,BYTE Debris4);
	void ArchangelsHammerMIX(LPOBJ lpObj);
	void BlessedDivineSwordOfArchangelMix(LPOBJ lpObj);
	void DarkAngelArmsMix(LPOBJ lpObj);
	void HolyangelWeaponMix(LPOBJ lpObj);
	void DivineAngelItemMix(LPOBJ lpObj);
	void FEItemChange(LPOBJ lpObj,int type);
	void MagicStonemMix(LPOBJ lpObj, BYTE count);
	void BoundWings2ndMix(LPOBJ lpObj);
	void BoundWings3rdMix(LPOBJ lpObj);
	void MGItemChange(LPOBJ lpObj,int type);

public:
	int m_SeniorMixLimitDay;
	int m_SeniorMixLimitMonth;
	int m_SeniorMixLimitYear;
};

extern CChaosBox gChaosBox;
