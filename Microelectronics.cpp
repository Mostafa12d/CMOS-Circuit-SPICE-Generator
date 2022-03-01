#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
int noMOSFETs = 1;
int noWires = 1;
string NOT(string input, string& output)
{
	string expression = ""; // Netlist inverter expression
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input + " vdd	vdd	PMOS \n"; //PMOS in the inverter
	noMOSFETs++; // increment number of MOSFETs

	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input + " 0	0	NMOS \n"; //NMOS
	noMOSFETs++; 
	noWires++; // increment the number of wires
	output = "W" + to_string(noWires-1); // number of wires to be used in display
	return expression;
}

string AND(string input1, string input2, string& output)
{
	// name drain gate source body type
	string expression = ""; 
	string NOTout;
	int temp;
	// M1 W1 a 0 0 NMOS (pull down network 1)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input1 + " 0   0   NMOS \n";
	noMOSFETs++;
	//temp = noWires;
	noWires++; 
	temp = noWires;
	// M2 W2 b W1 W1 NMOS (pull down network 2)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input2 + " W" + to_string(noWires - 1) + " W" + to_string(noWires - 1) + " NMOS \n";
	noMOSFETs++;
	//M3 W2 a vdd vdd PMOS(pull up network 1)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input1 + " vdd   vdd   PMOS \n";
	noMOSFETs++;
	//M4 W2 b vdd vdd PMOS(pull up network 1)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input2 + " vdd   vdd   PMOS \n";
	noMOSFETs++;
	noWires++;
	expression += NOT("W" + to_string(temp), NOTout);
	output = NOTout;
	return expression;
}

string OR(string input1, string input2, string& output)
{
	// name drain gate source body type
	string expression = "";
	string NOTout;
	int temp;
	// M1 W1 a 0 0 NMOS (pull down network 1)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input1 + " 0   0   NMOS \n";
	noMOSFETs++;
	// M2 W1 b 0 0 NMOS (pull down network 2)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input2 + " 0   0   NMOS \n";
	noMOSFETs++;
	temp = noWires;
	noWires++;
	//M3 W1 b W2 W2 PMOS(pull up network 1)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires - 1) + " " + input2 + " W" + to_string(noWires) + " W" + to_string(noWires) + " PMOS \n";
	noMOSFETs++;
	//M4 W2 a vdd vdd PMOS(pull up network 2)
	expression += "M" + to_string(noMOSFETs) + " W" + to_string(noWires) + " " + input1 + " vdd   vdd   PMOS \n";
	noMOSFETs++;
	noWires++;
	expression += NOT("W" + to_string(temp), NOTout);
	output = NOTout;
	return expression;
}


bool validate(string expression)
{
	
		for (int j = 2; j < expression.size(); j++)
		{
			if (expression[0] == expression[j])
			{
				return false;
			}
		}
	return true;
}

void generateExpression(vector<string> expression)
{
	//vector<string> expressions = getExpressions();
	string tempWire; // output for each boolean operation
	string tempOutput; // this holds the output wire for each expression
	string input; // expression excluding "y ="
	string tempInput = input;


	for (int i = 0; i < expression.size(); i++)
	{
		//NOT
		input = expression[i].substr(2); // size = 7
		for (int j = 0; j < input.size(); j++) // ab+c'+a ab+w20+a
		{
			if (input[j] == '\'')
			{
				string temp = "";
				temp += input[j - 1];
				cout << NOT(temp, tempWire);
				input.replace(j - 1, 2, tempWire);
			}
		}
		// AND
		for (int k = 0; k < input.size(); k++)
		{
			if (input[k] == '&' || input[k] == '.')
			{

				string temp = "";
				string temp1 = "";
				string temp2 = "";
				int flag1 = 0, flag2 = 0;
				int x = k;
				if (x > 0 && (int)input[x - 1] >= 48 && (int)input[x - 1] <= 57)
				{
					while (x > 0 && (int)input[x - 1] >= 48 && (int)input[x - 1] <= 57)
					{
						x--;
						temp += input[x];
					}
					temp += 'W';
					reverse(temp.begin(), temp.end());
					temp1 = temp;
					flag1 = x - 1;
				}
				else
				{
					temp1 += input[k - 1];
					flag1 = k - 1;
				}

				x = k + 1;
				if (input[k + 1] == 'W')
				{
					temp2 += "W";
					while (x < input.size() && (int)input[x + 1] >= 48 && (int)input[x + 1] <= 57)
					{
						x++;
						temp2 += input[x];
					}
					flag2 = x;
				}
				else
				{
					temp2 += input[k + 1];
					flag2 = k + 1;
				}

				cout << AND(temp1, temp2, tempWire);

				input.replace(flag1, flag2 - flag1 + 1, tempWire);

			}
		}
		//OR
		for (int k = 0; k < input.size(); k++)
		{

			if (input[k] == '|' || input[k] == '+')
			{

				string temp = "";
				string temp1 = "";
				string temp2 = "";
				int flag1 = 0, flag2 = 0;
				int x = k;
				if (x > 0 && (int)input[x - 1] >= 48 && (int)input[x - 1] <= 57)
				{
					while (x > 0 && (int)input[x - 1] >= 48 && (int)input[x - 1] <= 57)
					{
						x--;
						temp += input[x];
					}
					temp += 'W';
					reverse(temp.begin(), temp.end());
					temp1 = temp;
					flag1 = x - 1;
				}
				else
				{
					temp1 += input[k - 1];
					flag1 = k - 1;
				}

				x = k+1;
				if (input[k + 1] == 'W')
				{
					temp2 += "W";
					while (x < input.size() && (int)input[x + 1] >= 48 && (int)input[x + 1] <= 57)
					{
						x++;
						temp2 += input[x];
					}
					flag2 = x;
				}
				else
				{
					temp2 += input[k + 1];
					flag2 = k + 1;
				}

				cout << OR(temp1, temp2, tempWire);

				input.replace(flag1, flag2 - flag1 + 1, tempWire);

			}
		}
	}
	if (!input.find('\'') && !input.find('|') && !input.find('&') && !input.find('+') && !input.find('.'))
		tempOutput = tempWire;
}
void getExpressions()
{
	string input; // raw input expression
	string temp;
	vector<string> expressions;

	cout << "Please enter input" << endl;
	getline(cin, input);

	stringstream s(input);

	if (input.find(';'))
	{
		while (getline(s, temp, ';'))
		{
			while (!validate(temp))
			{
				cout << "The expression " << temp << " is invalid." << endl;
				cout << "Enter a valid expression: ";
				getline(cin, temp);
			}
			expressions.push_back(temp);
		}
	}
	else
	{
		while (!validate(input))
		{
			cout << "The expression " << input << " is invalid." << endl;
			cout << "Enter a valid expression: ";
			getline(cin, input);
		}
		expressions.push_back(input);
	}

	generateExpression(expressions);
}

int main()
{
	getExpressions();
}