/*
 * Copyright 2015 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of UltimateCodeSearch.
 *
 * UltimateCodeSearch is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * UltimateCodeSearch is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * UltimateCodeSearch.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file */

#include "TypeManager.h"

#include <algorithm>

struct Type
{
	/// The name of the type.
	std::string m_type_name;

	/// Vector of the extensions, literal strings, and first-line regexes which match the type.
	std::vector<std::string> m_type_extensions;
};

static const Type f_builtin_type_array[] =
{
	{ "actionscript", {".as", ".mxml"} },
	{ "ada", {".ada", ".adb", ".ads"} },
	{ "asm", {".asm", ".s"} },
	{ "asp", {".asp"} },
	{ "aspx", {".master", ".ascx", ".asmx", ".aspx", ".svc"} },
	{ "batch", {".bat", ".cmd"} },
	{ "cc", {".c", ".h", ".xs"} },
	{ "cfmx", {".cfc", ".cfm", ".cfml"} },
	{ "clojure", {".clj"} },
	{ "cmake", {"CMakeLists.txt", ".cmake"} },
	{ "coffeescript", {".coffee"} },
	{ "cpp", {".cpp", ".cc", ".cxx", ".m", ".hpp", ".hh", ".h", ".hxx"} },
	{ "csharp", {".cs"} },
	{ "css", {".css"} },
	{ "dart", {".dart"} },
	{ "delphi", {".pas", ".int", ".dfm", ".nfm", ".dof", ".dpk", ".dproj", ".groupproj", ".bdsgroup", ".bdsproj"} },
	{ "elisp", {".el"} },
	{ "elixir", {".ex", ".exs"} },
	{ "erlang", {".erl", ".hrl"} },
	{ "fortran", {".f", ".f77", ".f90", ".f95", ".f03", ".for", ".ftn", ".fpp"} },
	{ "go", {".go"} },
	{ "groovy", {".groovy", ".gtmpl", ".gpp", ".grunit", ".gradle"} },
	{ "haskell", {".hs", ".lhs"} },
	{ "hh", {".h"} },
	{ "html", {".htm", ".html"} },
	{ "jade", {".jade"} },
	{ "java", {".java", ".properties"} },
	{ "js", {".js"} },
	{ "json", {".json"} },
	{ "jsp", {".jsp", ".jspx", ".jhtm", ".jhtml"} },
	{ "less", {".less"} },
	{ "lisp", {".lisp", ".lsp"} },
	{ "lua", {".lua", R"(/^#!.*\blua(jit)?/)"} },
	{ "make", {".mk", ".mak", "makefile", "Makefile", "Makefile.Debug", "Makefile.Release"} },
	{ "matlab", {".m"} },
	{ "objc", {".m", ".h"} },
	{ "objcpp", {".mm", ".h"} },
	{ "ocaml", {".ml", ".mli"} },
	{ "parrot", {".pir", ".pasm", ".pmc", ".ops", ".pod", ".pg", ".tg"} },
	{ "perl", {".pl", ".pm", ".pod", ".t", ".psgi", R"(/^#!.*\bperl/)"} },
	{ "perltest", {".t"} },
	{ "php", {".php", ".phpt", ".php3", ".php4", ".php5", ".phtml", R"(/^#!.*\bphp/)"} },
	{ "plone", {".pt", ".cpt", ".metadata", ".cpy", ".py"} },
	{ "python", {".py", R"(/^#!.*\bpython/)"} },
	{ "rake", {"Rakefile"} },
	{ "rr", {".R"} },
	{ "rst", {".rst"} },
	{ "ruby", {".rb", ".rhtml", ".rjs", ".rxml", ".erb", ".rake", ".spec", "Rakefile", R"(/^#!.*\bruby/)"} },
	{ "rust", {".rs"} },
	{ "sass", {".sass", ".scss"} },
	{ "scala", {".scala"} },
	{ "scheme", {".scm", ".ss"} },
	{ "shell", {".sh", ".bash", ".csh", ".tcsh", ".ksh", ".zsh", ".fish", R"(/^#!.*\b(?:ba|t?c|k|z|fi)?sh\b/)"} },
	{ "smalltalk", {".st"} },
	{ "smarty", {".tpl"} },
	{ "sql", {".sql", ".ctl"} },
	{ "stylus", {".styl"} },
	{ "tcl", {".tcl", ".itcl", ".itk"} },
	{ "tex", {".tex", ".cls", ".sty"} },
	{ "tt", {".tt", ".tt2", ".ttml"} },
	{ "vb", {".bas", ".cls", ".frm", ".ctl", ".vb", ".resx"} },
	{ "verilog", {".v", ".vh", ".sv"} },
	{ "vhdl", {".vhd", ".vhdl"} },
	{ "vim", {".vim"} },
	{ "xml", {".xml", ".dtd", ".xsl", ".xslt", ".ent", R"(/<[?]xml/)"} },
	{ "yaml", {".yaml", ".yml"} },
	{ "", {""} }
};


TypeManager::TypeManager()
{
	size_t i = 0;
	Type t;

	// Populate the type map with the built-in defaults.
	while(t = f_builtin_type_array[i], !t.m_type_name.empty())
	{
		m_type_to_extension_map[t.m_type_name] = t.m_type_extensions;
		++i;
	}
}

TypeManager::~TypeManager()
{
	// TODO Auto-generated destructor stub
}

bool TypeManager::FileShouldBeScanned(const std::string& path,
		const std::string& name)
{
	// Find the name's extension.
	const char period[] = ".";
	auto last_period = std::find_end(name.begin(), name.end(),
			period, period+1);
	if(last_period != name.end())
	{
		// There was a period, might be an extension.
		if(name[0] != '.')
		{
			// Name doesn't start with a period, it still could be an extension.
			auto ext = std::string(last_period, name.end());

			if(m_include_extensions.find(ext) != m_include_extensions.end())
			{
				// Found the extension in the hash of extensions to include.
				return true;
			}
		}
	}

	// Check if the filename is one of the literal filenames we're supposed to look at.
	if(m_included_literal_filenames.find(name) != m_included_literal_filenames.end())
	{
		return true;
	}

	/// @todo Support first-line regexes.

	return false;
}

void TypeManager::CompileTypeTables()
{
	for(auto i : m_type_to_extension_map)
	{
		for(auto j : i.second)
		{
			if(j[0] == '.')
			{
				// First char is a '.', this is an extension specification.
				m_include_extensions.emplace(j, i.first);
			}
			else if(j[0] == '/')
			{
				// First char is a '/', it's a first-line regex.
				m_included_first_line_regexes.emplace(j, i.first);
			}
			else
			{
				// It's a literal filename (e.g. "Makefile").
				m_included_literal_filenames.emplace(j, i.first);
			}
		}
	}
}
