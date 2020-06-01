#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <list>

/*
 * The regex looks like this:
 * (?:<(\w+)|(\w+) = ("\w+")|>|<\/\w+>)
 *
 */

/* Construct the regex for the problem:
 *
 * <tag1 attr1 = "value1" attr2 = "value2">
 * <tag2 attr3 = "value3">
 * <tag3>
 *
 * Are all valid strings that I need to parse.
 *
 * Obvious start is:
 * (<\w+|\w+ = "\w+")
 * a single capturing group which captures either
 * the <tagN part of the string, or the attrN = "valueN"
 * part, both as a whole.
 * Get the capturing group on the first token only, to
 * remove the "<" symbol:
 * (<(\w+)|\w+ = "\w+")
 * The first match has an outer match, i.e. <tagN, and an
 * inner match, i.e. tagN
 * I want to obtain something similar for the second match
 * as well to extract each submatch and remove the =:
 * (<(\w+)|(\w+) = ("\w+"))
 * To finalise, I am not interested in the outer group
 * captures, so I can tell to ignore them using ?:
 * (?:<(\w+)|(\w+) = ("\w+))
 * And cycle through all groups found for each match
 *
 * The capturing groups are numbered, so in case the outer
 * one is not ignored, there are 4 groups, 1 outer, and
 * 2, 3, 4 inner. In case I ignore the outer group, there
 * are three, called 1, 2, 3. The first one (at index 0)
 * is always the match result, i.e. <tagN or attrN = "valueN"
 *
 * NOTE: Added two more non-capturing groups to match end tag
 *       and the end of a normal tag line.
 */

struct Node {
  std::string tag;
  std::vector< std::pair<std::string, std::string> > attributes;
  Node *subnodes[50];
  size_t subnodes_size;
  Node *parent;
  Node(): subnodes{NULL}, subnodes_size(0), parent(NULL){};
};

static void print_Node(std::string prefix, Node *ptr)
{
  std::cout << prefix << ptr->tag << "(";
  auto it_attr = ptr->attributes.begin();
  while (it_attr != ptr->attributes.end()) {
    std::cout << (*it_attr).first << ": " << (*it_attr).second;
    if (!(it_attr+1 == ptr->attributes.end())) {
      std::cout << ", ";
    }
    it_attr++;
  }
  std::cout << ")" << std::endl;
#ifdef DEBUG
  std::cout << "DBG: This node " << ptr << " has " << ptr->subnodes_size << " subnodes" << std::endl;
#endif
  for (int i=0; i<ptr->subnodes_size; i++) {
    print_Node(prefix + "  ", ptr->subnodes[i]);
  }
}

static void print_parsed_structure(Node *parsed_structure, size_t parsed_structure_size)
{
  for (int i=0; i<parsed_structure_size; i++) {
#ifdef DEBUG
    std::cout << "DBG: Node " << &parsed_structure[i] << " has parent equal to " << parsed_structure[i].parent << std::endl;
#endif
    if (parsed_structure[i].parent == NULL) {
      print_Node("", &parsed_structure[i]);
    }
  }
}

static Node *matching_node(Node **subnodes, size_t subnodes_size, std::string tag)
{
  for (int i=0; i<subnodes_size; i++) {
    if (subnodes[i]->tag == tag) {
      return subnodes[i];
    }
  }
  return NULL;
}

static void check_parsed_structure(Node *parsed_structure, size_t parsed_structure_size, std::vector<std::string> tags, std::string req_field)
{
  for (int i=0; i<parsed_structure_size; i++) {
    if (parsed_structure[i].parent == NULL) {
      Node *n = &parsed_structure[i]; 
      for (int j=0; j<tags.size(); j++) {
	if (n = matching_node(n->subnodes, n->subnodes_size, tags[j])) {
#ifdef DEBUG
	  std::cout << "DBG: Tag " << tags[j] << " matches on node " << n << std::endl;
#endif
	}
      }
    }
  }
}

static std::string process_tags(std::string const& s, std::string::size_type *new_start)
{
  std::string::size_type pos = s.find('.');
  if (pos != std::string::npos)
  {
    *new_start = pos;
    return s.substr(0, pos);
  }
  else
  {
    *new_start = 0;
    return s;
  }
}

int main(int argc, char *argv[])
{
  std::string s(R"((?:<(\w+)|(\w+) = ("\w+")|>|<\/\w+>))");
  std::regex r(s);
  if (argc != 2) {
    std::cout << "Need to pass an input file" << std::endl;
    exit(1);
  }
  std::ifstream infile(argv[1]);
  if (!infile.is_open()) {
    std::cout << "Error opening " << argv[1] << std::endl;
    exit(1);
  }
  std::smatch res;
  std::string read_line;
  std::getline(infile, read_line);
  std::istringstream iss(read_line);
  int N, Q;
  iss >> N >> Q;
  std::cout << "N is " << N << " and Q is " << Q << std::endl;

  Node parsed_structure[20];
  size_t parsed_structure_size = 0;
  Node *current_parent = NULL;
  Node *previous_parent = NULL;

  for (int i = 0; i < N; i++) {
    bool end_tag = true;
    std::getline(infile, read_line);
    Node tmp;
    while (std::regex_search(read_line, res, r)) {

#ifdef DEBUG
      std::cout << "DBG: Match is " << res.str(0) << std::endl;
#endif

      if (!res.str(1).empty()) {

	tmp.tag = res.str(1);

      } else if (!res.str(2).empty() && !res.str(3).empty()) {

        tmp.attributes.emplace_back(make_pair(res.str(2), res.str(3)));

      } else if (res.str(0) == ">") {

	end_tag = false;

      } else {

	end_tag = true;

      }

      read_line = res.suffix().str();
    }

    if (end_tag) {
#ifdef DEBUG
      std::cout << "DBG: The previous match was an end tag" << std::endl;
#endif
    }
    
    tmp.parent = current_parent;
    if (!end_tag) {
      parsed_structure[parsed_structure_size++] = tmp;
      /* Add this node to the subnodes of what is pointed by current_parent */
      Node *ptr = &parsed_structure[parsed_structure_size-1];
#ifdef DEBUG
      std::cout << "DBG: Added " << ptr << "(" << ptr->tag << ")" << std::endl;
#endif
      if (current_parent) {
#ifdef DEBUG
	std::cout << "DBG: Adding " << ptr << " to " << current_parent << " subnodes list " << std::endl;
#endif
	current_parent->subnodes[current_parent->subnodes_size++] = ptr;
      }
      current_parent = ptr;
#ifdef DEBUG
      std::cout << "DBG: Current parent is now " << current_parent << std::endl;
#endif
    } else {
      
      current_parent = current_parent->parent;
#ifdef DEBUG
      std::cout << "DBG: Current parent is now " << current_parent << std::endl;
#endif
    }
  }

  /* Print the result of the parsing */
  print_parsed_structure(parsed_structure, parsed_structure_size);
  
  for (int i = 0; i < Q; i++) {
    std::getline(infile, read_line);
    std::string req_field = read_line.substr(read_line.find("~") + 1);
#ifdef DEBUG
    std::cout << "DBG: Requested field is " << req_field << std::endl;
#endif
    std::string::size_type new_start;
    std::string s;
    std::vector<std::string> tags;
    while ((s = process_tags(read_line, &new_start)) != read_line) {
#ifdef DEBUG
      std::cout << "DBG: tag is " << s << std::endl;
#endif
      tags.emplace_back(s);
      read_line = read_line.substr(new_start + 1);
#ifdef DEBUG
      std::cout << "DBG: new string to process is " << read_line << std::endl;
#endif
    }
    std::string last_tag = read_line.substr(0, read_line.find("~"));
    tags.emplace_back(last_tag);
#ifdef DEBUG
    std::cout << "DBG: last tag is " << last_tag << std::endl;
#endif
    /* Check if the request is correct or not, and print the result */
    check_parsed_structure(parsed_structure, parsed_structure_size, tags, req_field);
  }

  return 0;
}
