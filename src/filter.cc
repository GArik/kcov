#include <filter.hh>
#include <configuration.hh>

#include <limits.h>
#include <stdlib.h>
#include <string>

using namespace kcov;

class Filter : public IFilter
{
public:
	Filter()
	{
		m_patternHandler = new PatternHandler();
		m_pathHandler = new PathHandler();
	}

	~Filter()
	{
		delete m_patternHandler;
		delete m_pathHandler;
	}

	// Used by the unit test
	void setup()
	{
		delete m_patternHandler;
		delete m_pathHandler;
		m_patternHandler = new PatternHandler();
		m_pathHandler = new PathHandler();
	}

	bool runFilters(std::string file)
	{
		if (m_pathHandler->isSetup())
			return m_pathHandler->includeFile(file);

		if (m_patternHandler->isSetup())
			return m_patternHandler->includeFile(file);

		return true;
	}


private:
	class PatternHandler
	{
	public:
		PatternHandler() :
			m_includePatterns(IConfiguration::getInstance().getOnlyIncludePattern()),
			m_excludePatterns(IConfiguration::getInstance().getExcludePattern())
		{
		}

		bool isSetup()
		{
			return !(m_includePatterns.size() == 0 && m_excludePatterns.size() == 0);
		}

		bool includeFile(std::string file)
		{
			if (m_includePatterns.size() == 0 && m_excludePatterns.size() == 0)
				return true;

			bool out = true;

			if (m_includePatterns.size() != 0)
				out = false;

			for (std::map<unsigned int, std::string>::iterator it = m_includePatterns.begin();
					it != m_includePatterns.end();
					it++) {
				std::string &pattern = it->second;

				if (file.find(pattern) != std::string::npos)
					out = true;
			}

			for (std::map<unsigned int, std::string>::iterator it = m_excludePatterns.begin();
					it != m_excludePatterns.end();
					it++) {
				std::string &pattern = it->second;

				if (file.find(pattern) != std::string::npos)
					out = false;
			}

			return out;
		}
	private:
		std::map<unsigned int, std::string> &m_includePatterns;
		std::map<unsigned int, std::string> &m_excludePatterns;
	};


	class PathHandler
	{
	public:
		PathHandler() :
			m_includePaths(IConfiguration::getInstance().getOnlyIncludePath()),
			m_excludePaths(IConfiguration::getInstance().getExcludePath())
		{
		}

		bool isSetup()
		{
			return !(m_includePaths.size() == 0 && m_excludePaths.size() == 0);
		}

		bool includeFile(std::string file)
		{
			if (m_includePaths.size() == 0 && m_excludePaths.size() == 0)
				return true;

			bool out = true;

			if (m_includePaths.size() != 0)
				out = false;

			char *path = realpath(file.c_str(), NULL);
			// Can't show something which doesn't exist, so filter it out
			if (!path)
				return false;
			std::string pathStr(path);
			free(path);

			for (std::map<unsigned int, std::string>::iterator it = m_includePaths.begin();
					it != m_includePaths.end();
					it++) {
				std::string &pathPattern = it->second;

				if (pathStr.find(pathPattern) == 0)
					out = true;
			}

			for (std::map<unsigned int, std::string>::iterator it = m_excludePaths.begin();
					it != m_excludePaths.end();
					it++) {
				std::string &pathPattern = it->second;

				if (pathStr.find(pathPattern) == 0)
					out = false;
			}

			return out;
		}
	private:
		std::map<unsigned int, std::string> &m_includePaths;
		std::map<unsigned int, std::string> &m_excludePaths;
	};


	PatternHandler *m_patternHandler;
	PathHandler *m_pathHandler;
};


IFilter &IFilter::getInstance()
{
	static Filter *g_instance;

	if (!g_instance)
		g_instance = new Filter();

	return *g_instance;
}
