// Copyright 2016-2017 Diffblue Limited. All Rights Reserved.

/// \file
/// Symbol table

/// \defgroup gr_symbol_table Symbol Table

#ifndef CPROVER_UTIL_SYMBOL_TABLE_H
#define CPROVER_UTIL_SYMBOL_TABLE_H

#include <iosfwd>
#include <map>
#include <unordered_map>

#include <util/optional.h>

#include "symbol.h"


#define forall_symbols(it, expr) \
  for(symbol_tablet::symbolst::const_iterator it=(expr).begin(); \
      it!=(expr).end(); ++it)

typedef std::multimap<irep_idt, irep_idt> symbol_base_mapt;
typedef std::multimap<irep_idt, irep_idt> symbol_module_mapt;

#define forall_symbol_base_map(it, expr, base_name) \
  for(symbol_base_mapt::const_iterator it=(expr).lower_bound(base_name), \
                                       it_end=(expr).upper_bound(base_name); \
      it!=it_end; ++it)

#define forall_symbol_module_map(it, expr, module) \
  for(symbol_module_mapt::const_iterator it=(expr).lower_bound(module), \
                                         it_end=(expr).upper_bound(module); \
      it!=it_end; ++it)


/// \brief The symbol table
/// \ingroup gr_symbol_table
class symbol_tablet
{
public:
  typedef std::unordered_map<irep_idt, symbolt, irep_id_hash> symbolst;

private:
  symbolst internal_symbols;
  symbol_base_mapt internal_symbol_base_map;
  symbol_module_mapt internal_symbol_module_map;

public:
  const symbolst &symbols;
  const symbol_base_mapt &symbol_base_map;
  const symbol_module_mapt &symbol_module_map;

public:
  symbol_tablet()
    : symbols(internal_symbols),
      symbol_base_map(internal_symbol_base_map),
      symbol_module_map(internal_symbol_module_map)
  {
  }

  symbol_tablet(const symbol_tablet &other)
  : internal_symbols(other.symbols),
    internal_symbol_base_map(other.symbol_base_map),
    internal_symbol_module_map(other.symbol_module_map),
    symbols(internal_symbols),
    symbol_base_map(internal_symbol_base_map),
    symbol_module_map(internal_symbol_module_map)
  {
  }

  symbol_tablet &operator=(const symbol_tablet &other)
  {
    // Copy to temp and then call move assignment
    return *this=symbol_tablet(other);
  }

  symbol_tablet(symbol_tablet &&other)
  : internal_symbols(std::move(other.internal_symbols)),
    internal_symbol_base_map(std::move(other.internal_symbol_base_map)),
    internal_symbol_module_map(std::move(other.symbol_module_map)),
    symbols(internal_symbols),
    symbol_base_map(internal_symbol_base_map),
    symbol_module_map(internal_symbol_module_map)
  {
  }

  symbol_tablet &operator=(symbol_tablet &&other)
  {
    internal_symbols=std::move(other.internal_symbols);
    internal_symbol_base_map=std::move(other.internal_symbol_base_map);
    internal_symbol_module_map=std::move(other.symbol_module_map);
    return *this;
  }

  void swap(symbol_tablet &other)
  {
    internal_symbols.swap(other.internal_symbols);
    internal_symbol_base_map.swap(other.internal_symbol_base_map);
    internal_symbol_module_map.swap(other.internal_symbol_module_map);
  }

public:
  bool has_symbol(const irep_idt &name) const
  {
    return symbols.find(name)!=symbols.end();
  }

  /// Find a symbol in the symbol table for read-only access.
  /// \param identifier: The name of the symbol to look for
  /// \return an optional pointer, set if found, nullptr otherwise.
  const symbolt *lookup(const irep_idt &identifier) const
  {
    symbolst::const_iterator it=symbols.find(identifier);
    if(it==symbols.end())
      return nullptr;
    return &it->second;
  }

  /// Find a symbol in the symbol table for read-only access.
  /// \param identifier: The name of the symbol to look for
  /// \return A reference to the symbol
  /// \throw `std::out_of_range` if no such symbol exists
  const symbolt &lookup_ref(const irep_idt &identifier) const
  { return symbols.at(identifier); }

  /// Find a symbol in the symbol table for read-write access.
  /// \param identifier: The name of the symbol to look for
  /// \return an optional pointer, set if found, nullptr otherwise.
  symbolt *get_writeable(const irep_idt &identifier)
  {
    symbolst::iterator it=internal_symbols.find(identifier);
    if(it==symbols.end())
      return nullptr;
    return &it->second;
  }

  /// Find a symbol in the symbol table for read-write access.
  /// \param identifier: The name of the symbol to look for
  /// \return A reference to the symbol
  /// \throw `std::out_of_range` if no such symbol exists
  symbolt &get_writeable_ref(const irep_idt &identifier)
  {
    symbolt *symbol=get_writeable(identifier);
    if(symbol==nullptr)
      throw std::out_of_range("identifier not found in symbol_table");
    return *symbol;
  }

  bool add(const symbolt &symbol);
  /// Move or copy a new symbol to the symbol table
  /// \remark: This is a nicer interface than move and achieves the same
  /// result as both move and add
  /// \param symbol: The symbol to be added to the symbol table - can be
  /// moved or copied in
  /// \return Returns a reference to the newly inserted symbol or to the
  /// existing symbol if a symbol with the same name already exists in the
  /// symbol table, along with a bool that is true if a new symbol was inserted.
  std::pair<symbolt &, bool> insert(symbolt symbol);
  bool move(symbolt &symbol, symbolt *&new_symbol);

  bool remove(const irep_idt &name);
  /// Remove a symbol from the symbol table
  /// \param entry: an iterator pointing at the symbol to remove
  void erase(const symbolst::const_iterator &entry);
  void clear()
  {
    internal_symbols.clear();
    internal_symbol_base_map.clear();
    internal_symbol_module_map.clear();
  }

  void show(std::ostream &out) const;
};

std::ostream &operator<<(
  std::ostream &out,
  const symbol_tablet &symbol_table);

#endif // CPROVER_UTIL_SYMBOL_TABLE_H
