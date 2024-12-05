#ifndef ENTRY_H
#define ENTRY_H

#include "common.h"
struct Table;
#include <variant>
#include <memory>
#include "list.h"

// an entry in the table
struct Entry {
	TYPE type;
	// vectors and tables have constructors and destructors, so simple union cannot be used, had to use variant
		// HOW DO THESE POINTERS GET FREED WTF
	std::variant<
			APPLY_DATA, // case of apply
			APPLY_LIST_DATA, // case of apply list. not a List because easier this way, no other reason, might change
			SUB_DATA, // case of subtable is represented as a new table. pointer due to circular dependency
			LIST_DATA // case of being a list, cannot be a table unfortunately. could be object and not pointer, but this makes it consistent
	> data;
	int active_theme; // theme selected as active for this entry

	Entry(const std::string &name, std::string &line, const std::string &dataDir, int numThemes); // receives line, parses it
	Entry() = default;
	~Entry() = default;
	void parseApply(std::string &line);
	void parseApplyList(std::string &line);
	void parseSub(const std::string &name, const std::string dataDir, int numThemes);
	void parseList(std::string &line, bool show_pictures);

	void print(int depth_level) const;
	// same as table, go read
	std::string read(std::string &input) const;

	// 2 methods too allow to distinguish between 'selecting' option or just reading it
	std::string menu(const std::string &name, const std::string &info, const std::vector<std::string> &color_icons) const; // assumes read only
	std::string print_option(const std::string &name, const std::string &info, const std::vector<std::string> &color_icons) const; // doubles for apply_list
	// std::string menu(const std::string &name, int theme, std::string &input, std::string &info, const std::vector<std::string> &color_icons);

	void applyAll(int theme, int numThemes);
	bool applyHasDataFor(int theme) const;
	bool applyListHasDataFor(int theme) const;
};

#endif
