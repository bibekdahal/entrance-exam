#pragma once

const int g_maxQ = 65;


class User
{
private:
	std::string m_name;
	std::string m_username;
	std::string m_pass;
	bool m_isLoggedIn;
public:
	User()
	{
		m_isLoggedIn = false;
	}
	bool LoggedIn()
	{
		return false;
	}
	bool LogIn(std::string username, std::string password)
	{
		m_username = username;
		m_pass = password;
		return true;
	}

    std::string UserName() { return m_username; }
};