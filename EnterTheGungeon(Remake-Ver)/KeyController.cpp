#include "KeyController.h"

/*
* GetAsyncKeyState() �Լ� ��ȯ ��
0 (0x0000)
������ ���� ���� ���� ȣ�� �������� �ȴ��� ����

0x8000 = -32768
������ ���� ���� ���� ȣ�� �������� ���� ����

0x8001 = -32767
������ ���� ���� �ְ� ȣ�� �������� ���� ����
-> (�߰��� TMI) �ش� Ű�� GetAsyncKeyState�� ���� �� �� ȣ�� �� 0x8001�� ��ȯ�Ѵ�.

1 (0x0001)
������ ���� ���� �ְ� ȣ�� �������� �ȴ��� ����

summary
������ ���� ���� ���� ȣ�� �������� ���� ����, �� Ű�� ������ �������� 0x8000�� ��ȯ�մϴ�. 
���� �������� ���� ���� �ְ� ȣ�� ���������� �������� 0x8001�� ��ȯ�մϴ�. 
������ Ű�� �������� 0x0001�� ��ȯ�ϰ�, ������ �ʾҴٸ� 0x0000�� ��ȯ�մϴ�. 
Ȥ��, GetAsyncKeyState(VK_UP) & 0x8000 �̷� �ڵ带 �������� �����Ű���? 
0x8000�� GetAsyncKeyState ��ȯ���� AND �����ϴ� ������ ��Ȯ�� �������� Ű�� ���¸� Ȯ���ϱ� �����Դϴ�. 
���࿡ Ű�� ���� GetAsyncKeyState �Լ��� 0x8000�� ��ȯ�Ѵٸ� �̸� 0x8000���� AND �����Ͽ� Ű�� ���������� 0x8000�� ���ɴϴ�. 
�׷��� ������ ���� ������ GetAsyncKeyState �Լ��� 0x0001�� ��ȯ�Ѵٰ� �߾����ϴ�. 
0x0001���� 0x8000�� AND �����ϰԵǸ� 0�� ������ �˴ϴ�. (���࿡ AND �������� �ʾ��� ��쿡�� ������ ���� �ִ� ���µ� ���� �Ǿ� ������ ������ ���Դϴ�.
*/

/*
* GetAsyncKeyState(key) �Լ� ����
* input key�� �����ٸ� DEC -32767�� ��ȯ�մϴ�. (bit 16�ڸ�)
* ������ �ʾҴٸ� 0�� ��ȯ�մϴ�.
* 
* ��Ʈ ���� & ����
* key�� ������ �� GetAsyncKeyState ���� �� -32767 & 0x8000(DEC 32768) = 1�� ��ȯ�մϴ�.
*/

bitset<KeyMaxCount> KeyController::KeyUp;
bitset<KeyMaxCount> KeyController::KeyDown;

void KeyController::init()
{
    KeyUp.reset();
    KeyDown.reset();
}

bool KeyController::isOnceKeyDown(int16 key)
{
    switch (static_cast<bool>(GetAsyncKeyState(key) & 0x8000))
    {
    case true:
        if (KeyDown[key] == false) // Ű�� ������ ������ ���� ����
        {
            KeyDown.set(key, true); // ���� ���·� ��ȯ
            return true;
        }
        break;
    case false:
        KeyDown.set(key, false);
        break;
    }

    return false;
}

bool KeyController::isOnceKeyUp(int16 key)
{
    /*
    * �Լ� ȣ�� �� ó������ keyUp = true���� �ٲٰ� false�� return �Ѵ�.
    * �� ���� �� �� ���� �� ȣ�� �� Ű�� ���� ���� �����Ƿ� false�� ���鼭 keyUp�� true���� ������ ���� �ִٴ� ���� �˰� false �ʱ�ȭ ���� true ���� �մϴ�.
    * 
    * �� �� ���� ���� ���� ��� �ʹٸ� onceKeyUp�� ����.
    */

    switch (static_cast<bool>(GetAsyncKeyState(key) & 0x8000))
    {
    case true:
        KeyUp.set(key, true); // Ű�� �����ٸ� ó���� keyUp ���� ���� true���� �ٲٰ� false �����մϴ�.
        break;
    case false:
        if (KeyUp[key] == true) // �ٽ� �� �� ���Ƽ� �˻縦 �� �� ���� �� keyUp�� true �� ���� ���� �ִٸ�
        {
            KeyUp.set(key, false); // false�� �ʱ�ȭ�� �ϰ� return true�� �մϴ�.
            return true;
        }
        break;
    }

    return false;
}

bool KeyController::isStayKeyDown(int16 key)
{
    if (GetAsyncKeyState(key) & 0x8000)
        return true;

    return false;
}

bool KeyController::isToggleKey(int16 key)
{
    // Ű�� �����ٸ� -32767�� ��ȯ�մϴ�. �׸��� & 0x0001 ���� �� 1�� ��ȯ�մϴ�.
    // Ű�� ������ �ʾҴٸ� 0�� ��ȯ�մϴ�. �׸��� & 0x0001 ���� �� 0�� ��ȯ�մϴ�.

    // �����ڸ��� ������ ��� �ʹٸ� toggleKey�� ����Ѵ�.

    // And& 
    // 0000 0000 0000 0001 ������ Ű�� �������� 0x0001 (���� �� ȣ��� �̹� ȣ�� ���̿� Ű�� ������ ���� �־���.)
    // 0000 0000 0000 0001 0x0001
    // 0000 0000 0000 0001
    return (GetAsyncKeyState(key) & 0x0001) == true ? true : false;
}
