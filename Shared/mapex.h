// mapex.h: interface for the CMapEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPEX_H__44E4FC2A_83C2_49EE_A784_4D1584CD5339__INCLUDED_)
#define AFX_MAPEX_H__44E4FC2A_83C2_49EE_A784_4D1584CD5339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

#include "Misc.h"

//////////////////////////////////////////////////////////////////////

template <class KEY, class ARG_KEY>
class CSetBase : public CMap<KEY, ARG_KEY, char, char&>
{
public:
	CSetBase() {}

	CSetBase(const CSetBase& other)
	{
		Copy(other);
	}

	CSetBase(const CArray<KEY, KEY&>& other)
	{
		CopyFrom(other);
	}

	CSetBase(const KEY* pOther, int nNumOther)
	{
		CopyFrom(pOther, nNumOther);
	}

	KEY GetNextKey(POSITION& pos) const
	{
		char val = 0;
		KEY key;

		GetNextAssoc(pos, key, val);
		return key;
	}

	BOOL HasKey(KEY key) const
	{
		char val = 0;
		return Lookup(key, val);
	}

	void AddKey(KEY key)
	{
		char val = 0;
		SetAt(key, val);
	}

	void Copy(const CSetBase& other)
	{
		Misc::CopyT(other, *this);
	}

	BOOL MatchAll(const CSetBase& other) const
	{
		Misc::MatchAllT(other, *this);
	}

	int CopyFrom(const CArray<KEY, KEY&>& other)
	{
		return CopyFrom(other.GetData(), other.GetSize());
	}

	int CopyFrom(const KEY* pOther, int nNumOther)
	{
		if (!pOther)
		{
			ASSERT(0);
			return 0;
		}

		RemoveAll();
		int nItem = nNumOther;

		while (nItem--)
			AddKey(pOther[nItem]);

		return GetCount();
	}

	int CopyTo(CArray<KEY, KEY&>& other) const
	{
		CopyTo(other);
	}

protected:
	template <class ARRAY>
	int CopyFrom(const ARRAY& other)
	{
		return CopyFrom(other.GetData(), other.GetSize());
	}

	template <class ARRAY>
	int CopyTo(ARRAY& other) const
	{
		other.RemoveAll();
		POSITION pos = GetStartPosition();

		while (pos)
			other.Add(GetNextKey(pos));

		return other.GetSize();
	}
};

//////////////////////////////////////////////////////////////////////

template <class KEY>
class CSet : public CSetBase<KEY, KEY>
{
public:
	CSet() {}
	CSet(const CSet& other) : CSetBase(other) {}
	CSet(const CArray<KEY, KEY&>& other) : CSetBase(other) {}
	CSet(const KEY* pOther, int nNumOther) : CSetBase(pOther, nNumOther) {}

};

//////////////////////////////////////////////////////////////////////

class CDWordSet : public CSet<DWORD>
{
public:
	CDWordSet() {}
	CDWordSet(const CDWordSet& other) : CSet(other) {}
	CDWordSet(const CArray<DWORD, DWORD&>& other) : CSet(other) {}
	CDWordSet(const DWORD* pOther, int nNumOther) : CSet(pOther, nNumOther) {}
	CDWordSet(const CDWordArray& other) { CopyFrom(other); }

	int CopyFrom(const CDWordArray& other) { return CSet::CopyFrom(other); }
	int CopyTo(CDWordArray& other) const { return CSet::CopyTo(other); }
};

class CUintSet : public CSet<UINT>
{
public:
	CUintSet() {}
	CUintSet(const CUintSet& other) : CSet(other) {}
	CUintSet(const CArray<UINT, UINT&>& other) : CSet(other) {}
	CUintSet(const UINT* pOther, int nNumOther) : CSet(pOther, nNumOther) {}
	CUintSet(const CUIntArray& other) { CopyFrom(other); }

	int CopyFrom(const CUIntArray& other) {	return CSet::CopyFrom(other); }
	int CopyTo(CUIntArray& other) const { return CSet::CopyTo(other); }
};

class CStringSet : public CSetBase<CString, LPCTSTR>
{
public:
	CStringSet() {}
	CStringSet(const CStringSet& other) { Misc::CopyStrT<char>(other, *this); }
	CStringSet(const CArray<CString, CString&>& other) : CSetBase(other) {}
	CStringSet(const CString* pOther, int nNumOther) : CSetBase(pOther, nNumOther) {}
	CStringSet(const CStringArray& other) { CopyFrom(other); }

	int CopyFrom(const CStringArray& other) { return CSetBase::CopyFrom(other); }
	int CopyTo(CStringArray& other) const { return CSetBase::CopyTo(other); }
};

//////////////////////////////////////////////////////////////////////

template <class VALUE>
class CMapStringToContainer : protected CMap<CString, LPCTSTR, VALUE*, VALUE*&>
{
public:
	CMapStringToContainer()
	{
	}

	CMapStringToContainer(UINT hashSize)
	{
		if (hashSize)
			InitHashTable(hashSize);
	}
	
	virtual ~CMapStringToContainer()
	{
		RemoveAll();
	}

	POSITION GetStartPosition() const
	{
		return CMap<CString, LPCTSTR, VALUE*, VALUE*&>::GetStartPosition();
	}

	void GetNextAssoc(POSITION& rNextPosition, CString& rKey, VALUE*& rValue) const
	{
		CMap<CString, LPCTSTR, VALUE*, VALUE*&>::GetNextAssoc(rNextPosition, rKey, rValue);
	}
	
	const VALUE* GetMapping(const CString& str) const
	{
		return const_cast<CMapStringToContainer*>(this)->GetMapping(str);
	}
	
	BOOL HasMapping(const CString& str) const
	{
		return (GetMapping(str) != NULL);
	}
	
	VALUE* GetMapping(const CString& str)
	{
		VALUE* pMapping = NULL;
		
		if (!Lookup(str, pMapping))
			return NULL;
		
		ASSERT(pMapping);
		return pMapping;
	}

	VALUE* GetAddMapping(const CString& str)
	{
		VALUE* pMapping = NULL;
		
		if (Lookup(str, pMapping))
		{
			ASSERT(pMapping);
			return pMapping;
		}

		pMapping = new VALUE;
		SetAt(str, pMapping);

		return pMapping;
	}

	BOOL IsEmpty() const
	{
		return (!GetCount());
	}

	int GetCount() const
	{
		return CMap<CString, LPCTSTR, VALUE*, VALUE*&>::GetCount();
	}

	void RemoveAll()
	{
		CString str;
		VALUE* pMapping = NULL;

		POSITION pos = GetStartPosition();

		while (pos)
		{
			GetNextAssoc(pos, str, pMapping);

			ASSERT(pMapping);
			ASSERT(!str.IsEmpty());

			delete pMapping;
		}

		CMap<CString, LPCTSTR, VALUE*, VALUE*&>::RemoveAll();
	}
	
	void RemoveKey(const CString& str)
	{
		VALUE* pMapping = NULL;
		
		POSITION pos = GetStartPosition();
		
		if (Lookup(str, pMapping))
		{
			ASSERT(pMapping);
			delete pMapping;

			CMap<CString, LPCTSTR, VALUE*, VALUE*&>::RemoveKey(str);
		}
	}

	// overload new/delete because we've hidden the base class
	void* operator new (size_t size)
	{
		return CMap<CString, LPCTSTR, VALUE*, VALUE*&>::operator new(size);
	}

	void operator delete (void *p)
	{
		CMap<CString, LPCTSTR, VALUE*, VALUE*&>::operator delete(p);
	}

};

class CMapStringToStringArray : public CMapStringToContainer<CStringArray>
{
public:
	BOOL Map(const CString& str, const CStringArray& aItems)
	{
		CStringArray* pArray = GetAddMapping(str);

		if (!pArray)
		{
			ASSERT(0);
			return FALSE;
		}

		pArray->Copy(aItems);
		return TRUE;
	}

	void Copy(const CMapStringToStringArray& other)
	{
		RemoveAll();

		POSITION pos = other.GetStartPosition();
		CString sKey;
		CStringArray* pOtherArray = NULL;

		while (pos)
		{
			other.GetNextAssoc(pos, sKey, pOtherArray);
			ASSERT(pOtherArray);

			CStringArray* pArray = GetAddMapping(sKey);
			ASSERT(pArray);

			if (pArray && pOtherArray)
				pArray->Copy(*pOtherArray);
		}	
	}

	BOOL MatchAll(const CMapStringToStringArray& other, 
				  BOOL bOrderSensitive = FALSE, BOOL bCaseSensitive = FALSE) const
	{
		if (GetCount() != other.GetCount())
			return FALSE;

		POSITION pos = GetStartPosition();
		CString sKey;
		CStringArray* pArray = NULL;

		while (pos)
		{
			GetNextAssoc(pos, sKey, pArray);
			ASSERT(pArray);

			const CStringArray* pOtherArray = other.GetMapping(sKey);

			if (pOtherArray == NULL)
				return FALSE;
			
			if (!Misc::MatchAll(*pArray, *pOtherArray, bOrderSensitive, bCaseSensitive))
				return FALSE;
		}	

		return TRUE;
	}
};

typedef CMapStringToContainer<CMapStringToStringArray> CMapStringToStringArrayMap;

typedef CMapStringToContainer<CMapStringToString> CMapStringToStringMap;

#endif // !defined(AFX_MAPEX_H__44E4FC2A_83C2_49EE_A784_4D1584CD5339__INCLUDED_)
