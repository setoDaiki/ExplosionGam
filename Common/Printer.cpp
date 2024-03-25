#include "Printer.h"
#include <iostream>

using namespace std;

void Printer::PrintFloat3(DirectX::XMFLOAT3 float3) {
	cout << "x = " << float3.x << " y = " << float3.y << " z = " << float3.z << "\n";
}