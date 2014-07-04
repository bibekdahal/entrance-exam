#pragma once

const int g_maxQ = 65;

#include <iostream>

#include <fstream>
#include <vector>

class LoginData
{
	std::string uns[300];
	std::string pws[300];
	int n;
public:
	LoginData()
	{
		n = 0;
		std::ifstream ld;
		ld.open("uli.dat");
		for (int i = 0; i < 300; i++)
		{
			char pwb[10] = { 0 }, idb[10] = { 0 };
			ld.read(idb, 4);
			ld.read(pwb, 8);
			for (int j = 0; j < 8; j++)
			{
				pwb[j] = pwb[j] - 100;
			}
			for (int j = 0; j < 4; j++)
			{
				idb[j] = idb[j] - 100;
			}
			uns[i] = idb;
			pws[i] = pwb;
		}	
	}
	bool Find(std::string un, std::string pw)
	{
		for (int i = 0; i < 300; i++)
		{
			if (uns[i] == un && pws[i] == pw) return true;
		}
		return false;
	}
};

LoginData loginData;


class User
{
private:
	std::string m_name;
	std::string m_username;
	std::string m_pass;
	bool m_isLoggedIn;
	//bool m_completed = true;
public:
	User()
	{
		m_isLoggedIn = false;
	}
	bool LoggedIn()
	{
		return false;
	}
	bool LogIn(std::string name, std::string username, std::string password)
	{
		if (!loginData.Find(username, password)) return false;
		m_username = username;
		m_pass = password;
        m_name = name;
		return true;
	}

    std::string UserName() { return m_username; }
    std::string Name() { return m_name; }
};