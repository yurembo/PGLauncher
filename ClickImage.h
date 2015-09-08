// author: Yuriy Yazev, july 2015
#pragma once

#include <string>
// CClickImage

using namespace std;

class CClickImage : public CWnd
{
	DECLARE_DYNAMIC(CClickImage)

public:
	CClickImage();
	virtual ~CClickImage();

	int Up_img() const { return up_img; }
	void Up_img(int val) { up_img = val; }
	int Over_img() const { return over_img; }
	void Over_img(int val) { over_img = val; }
	int Press_img() const { return press_img; }
	void Press_img(int val) { press_img = val; }
	bool Over_flag() const { return over_flag; }
	void Over_flag(bool val) { over_flag = val; }
	bool Press_flag() const { return press_flag; }
	void Press_flag(bool val) { press_flag = val; }
	std::string Url() const { return url; }
	void Url(std::string val) { url = val; }

protected:
	static BOOL hasclass;
	static BOOL RegisterMe();
	CImage image;
	int up_img;
	int over_img;
	int press_img;
	bool over_flag;
	bool press_flag;
	string url;
	
	#define CLICKIMAGE_CLASS_NAME _T("ClickImage")

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void setImage(LPCWSTR fileName);
	void setImage(int picNum);
	void changeImage();
	bool saveDirPathToFile();
};