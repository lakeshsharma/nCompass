#if 000//TODO-PORT

#include "stdafx.h"
#include "UI\RadioButton.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
const CString GRAPHICSDIR = L"..\\..\\..\\Graphics\\";

namespace SentinelNativeUnitTests
{
	TEST_CLASS(RadioButtonUnitTests)
	{
	public:
		TEST_METHOD(RadioButtonConstructionUnitTest)
		{
			// Radio buttons are 20x20
			const int ID = 1;
			CRadioButtonInfo *bInfo = new CRadioButtonInfo();
			bInfo->SetFileNames(GRAPHICSDIR + L"CommonButtons\\tiny_radio_up.png", GRAPHICSDIR + L"CommonButtons\\tiny_radio_down.png");
			bInfo->SetID(ID);
			bInfo->SetGroupNum(0);
			bInfo->SetIsActive(true);
			bInfo->SetLocation(CRect(100,100,119,119));

			// Instantiate radio button
			CRadioButton *button = new CRadioButton(bInfo);
			Assert::IsNotNull(button);
			Assert::IsTrue(button->GetGroupNum() == 0, L"Incorrect group number");
			Assert::IsTrue(button->GetID() == ID, L"Incorrect ID");
			// Can't get button size without drawwing it. Drawing it requires a display context, which isn't available.
			// Assert::IsTrue(button->GetImageSize() == CSize(20,20), L"Incorrect size");

			delete button;
			delete bInfo;

		}

		TEST_METHOD(RadioButtonPressButtonUnitTest)
		{
			// Radio buttons are 20x20
			const int ID = 1;
			CRadioButtonInfo *bInfo = new CRadioButtonInfo();
			bInfo->SetFileNames(GRAPHICSDIR + L"CommonButtons\\tiny_radio_up.png", GRAPHICSDIR + L"CommonButtons\\tiny_radio_down.png");
			bInfo->SetID(ID);
			bInfo->SetGroupNum(0);
			bInfo->SetIsActive(false);
			bInfo->SetLocation(CRect(100,100,119,119));

			// Instantiate radio button
			CRadioButton *button = new CRadioButton(bInfo);

			// Press outside button area
			button->PressRadioButton(CPoint(125,105));
			Assert::IsFalse(button->IsActive(), L"Radio button pressed when outside button area");

			// Can't press inside button area because it invokes directory manager to get click sound file.

			delete button;
			delete bInfo;


		}

		TEST_METHOD(RadioButtonRectSurroundUnitTest)
		{
			// Radio buttons are 20x20
			const int ID = 1;
			CRadioButtonInfo *bInfo = new CRadioButtonInfo();
			bInfo->SetFileNames(GRAPHICSDIR + L"CommonButtons\\tiny_radio_up.png", GRAPHICSDIR + L"CommonButtons\\tiny_radio_down.png");
			bInfo->SetID(ID);
			bInfo->SetGroupNum(0);
			bInfo->SetIsActive(false);
			bInfo->SetLocation(CRect(100,100,119,119));

			// Instantiate radio button
			CRadioButton *button = new CRadioButton(bInfo);

			CRect fullyOutside(0,0,50,50);
			CRect fullyInside(101,101,102,102);
			CRect partialOutside(90,90,105,105);
			CRect fullySurround(95,95,125,125);
			CRect exact(100,100,119,119);

			Assert::IsTrue(button->RectSurroundsThis(fullySurround), L"Surrounding rect not indicated");
			Assert::IsTrue(button->RectSurroundsThis(exact), L"Exact same rect not indicated");
			Assert::IsFalse(button->RectSurroundsThis(fullyOutside), L"Fully outside rect indicated");
			Assert::IsFalse(button->RectSurroundsThis(fullyInside), L"Fully inside rect indicated");
			Assert::IsFalse(button->RectSurroundsThis(partialOutside), L"Partial outside rect indicated");

			delete button;
			delete bInfo;

		}
	};
}
#endif //TODO-PORT