#include "TrigonometricController.h"
#include "ScreenManager.h"
#include "MouseHandler.h"
#include "Debugging.h"

void TrigonometricController::init(POINT startPosition, POINT resolution, POINT halfResolution)
{
    _startPosition = move(startPosition);
    _destinationPosition = { _startPosition.x + resolution.x, _startPosition.y + resolution.y };

    _radiusOfCircle = halfResolution;
    _practicalRadius = { _startPosition.x + _radiusOfCircle.x, _startPosition.y + _radiusOfCircle.y };
    _upperLeftDegree = getDegree({ -halfResolution.x, -halfResolution.y });
    _upperRightDegree = getDegree({ halfResolution.x, -halfResolution.y });
    _lowerLeftDegree = getDegree({ -halfResolution.x, halfResolution.y });
    _lowerRightDegree = getDegree({ halfResolution.x, halfResolution.y });
}

void TrigonometricController::drawCircle(const HDC& hdc) const
{
    HPEN hPen, hPenOld;
    hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 125)); // �� ��Ÿ��, ����, ����
    hPenOld = (HPEN)::SelectObject(hdc, (HGDIOBJ)hPen); // �� ����

    //// X�� �׸���
    MoveToEx(hdc, _startPosition.x, _startPosition.y + _radiusOfCircle.y, nullptr);
    LineTo(hdc, _destinationPosition.x, _startPosition.y + _radiusOfCircle.y);

    //// Y�� �׸���
    MoveToEx(hdc, _startPosition.x + _radiusOfCircle.x, _startPosition.y, nullptr);
    LineTo(hdc, _startPosition.x + _radiusOfCircle.x, _destinationPosition.y);

    //// ���� �缱
    MoveToEx(hdc, _startPosition.x, _startPosition.y, nullptr);
    LineTo(hdc, _destinationPosition.x, _destinationPosition.y);

    //// ���� �缱
    MoveToEx(hdc, _destinationPosition.x, _startPosition.y, nullptr);
    LineTo(hdc, _startPosition.x, _destinationPosition.y);

    float radianOfCircle = 0.0f;
    MoveToEx(hdc, _startPosition.x + _radiusOfCircle.x + cos(radianOfCircle) * _radiusOfCircle.x, _startPosition.y + _radiusOfCircle.y + -sin(radianOfCircle) * _radiusOfCircle.y, nullptr);

    // ���� 1������ 1���� ������Ų��.
    // ���� 360���� ���� ������ �ݺ��ϴ� ������ 359���� ��ǥ���� ���� ������ 359���� 0��(360��) ���̰� �̾����� �ʾ�
    // ���� ���� �Ǳ� ������ 359���� 0��(360��)�� �̾��ֱ� ���ؼ��̴�.
    for (int i = 1; i <= 360; i++)
    {
        radianOfCircle = i * Radian;
        // �ش� ���� x, y��ǥ�� ����Ͽ� ���� �׸���.
        LineTo(hdc, _startPosition.x + _radiusOfCircle.x + cos(radianOfCircle) * _radiusOfCircle.x, _startPosition.y + _radiusOfCircle.y + -sin(radianOfCircle) * _radiusOfCircle.y);
    }

    hPen = (HPEN)::SelectObject(hdc, hPenOld); // ������ �� ����
    ::DeleteObject(hPen); // ������ �� ����

    const uint16 spX = ScreenManager::getInstance().getSubResolution().x - 200;
    Debugging::renderDouble(hdc, "����", 20, { spX, 50 }, WhiteRGB, true);
    Debugging::renderString(hdc, "����", 20, { spX, 70 }, WhiteRGB, true);
}

int16 TrigonometricController::getCurrentAngle() const
{
    int16 selectedAngle = -1;
    const double locatedDegree = getDegree(getDistance(MouseHandler::Mouse, _practicalRadius));
    Debugging::setPrintValue(locatedDegree);
    if (350 <= locatedDegree || locatedDegree <= 10)
    {
        Debugging::setPrintValue(" 0�� ����");
        selectedAngle = 0;
    }
    else if (10 <= locatedDegree && locatedDegree <= 70)
    {
        Debugging::setPrintValue(" 45�� ����");
        selectedAngle = 45;
    }
    else if (70 <= locatedDegree && locatedDegree <= 110)
    {
        Debugging::setPrintValue(" 90�� ����");
        selectedAngle = 90;
    }
    else if (110 <= locatedDegree && locatedDegree <= 170)
    {
        Debugging::setPrintValue(" 135�� ����");
        selectedAngle = 135;
    }
    else if (170 <= locatedDegree && locatedDegree <= 190)
    {
        Debugging::setPrintValue(" 180�� ����");
        selectedAngle = 180;
    }
    else if (190 <= locatedDegree && locatedDegree <= 240)
    {
        Debugging::setPrintValue(" 225�� ����");
        selectedAngle = 225;
    }
    else if (240 <= locatedDegree && locatedDegree <= 300)
    {
        Debugging::setPrintValue(" 270�� ����");
        selectedAngle = 270;
    }
    else if (300 <= locatedDegree && locatedDegree <= 350)
    {
        Debugging::setPrintValue(" 315�� ����");
        selectedAngle = 315;      
    }

    return selectedAngle;
}

int16 TrigonometricController::getPreciseCurrentAngle() const
{
    return getDegree(getDistance(MouseHandler::Mouse, _practicalRadius));
}

double TrigonometricController::getDegree(const pair<double, double>& pairForDoubleType) const
{
    /*
    * ������ x, y ��ǥ�� ��ũź��Ʈ, �� �� ź��Ʈ ���� ��ȯ�մϴ�.
    * ��ũź��Ʈ ���� ����(0, 0)���� ��ǥ(x_num, y_num)������ ���� X���� �̷�� �����Դϴ�.
    * �� ������ -pi�� pi ������ ����(-pi ����)���� ǥ�õ˴ϴ�.
    */
    double x = pairForDoubleType.first;
    double y = pairForDoubleType.second;
    const double radian = atan2((-y), x);
    double degree = Degree * radian;
    degree = radian < 0 ? degree + 360 : degree;
    return degree;
}

