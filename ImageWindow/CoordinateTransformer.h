#pragma once
class CCoordinateTransformer
{
public:
	inline BOOL IsAvailable() const	{return m_fScaleX != 0 && m_fScaleY != 0;}

	inline void Transform			(CPoint& pt) const
	{
		pt.x = TransformX(pt.x);
		pt.y = TransformY(pt.y);
	}

	inline void ReverseTransform	(CPoint& pt) const
	{
		pt.x = ReverseTransformX(pt.x);
		pt.y = ReverseTransformY(pt.y);
	}

	inline long TransformX(long x) const
	{
		return (long)((x - m_ptOffsetPre.x) * m_fScaleX + m_ptOffsetPost.x + 0.5);
	}

	inline long TransformY(long y) const
	{
		return (long)((y - m_ptOffsetPre.y) * m_fScaleY + m_ptOffsetPost.y + 0.5);
	}

	inline long ReverseTransformX(long x) const
	{
		return (long)((x - m_ptOffsetPost.x) / m_fScaleX + m_ptOffsetPre.x);
	}

	inline long ReverseTransformY(long y) const
	{
		return (long)((y - m_ptOffsetPost.y) / m_fScaleY + m_ptOffsetPre.y);
	}

	void SetOffsetPre	(const CPoint& offset);
	void SetOffsetPost	(const CPoint& offset);
	void SetScale		(double fScaleX, double fScaleY);

private:
	CPoint m_ptOffsetPre;
	CPoint m_ptOffsetPost;

	double m_fScaleX;
	double m_fScaleY;

public:
	CCoordinateTransformer(void);
	~CCoordinateTransformer(void);
};

