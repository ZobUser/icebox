#pragma once

#include "symbols.hpp"

#include <functional>

namespace memory { struct Io; }

namespace symbols
{
    struct Offset
    {
        std::string symbol;
        size_t      offset;
    };

    using on_symbol_fn = std::function<walk_e(const std::string&, size_t)>;

    struct Module
    {
        virtual ~Module() = default;

        virtual std::string_view    id              () = 0;
        virtual opt<size_t>         symbol_offset   (const std::string& symbol) = 0;
        virtual void                struc_names     (const symbols::on_name_fn& on_struc) = 0;
        virtual opt<size_t>         struc_size      (const std::string& struc) = 0;
        virtual void                struc_members   (const std::string& struc, const symbols::on_name_fn& on_member) = 0;
        virtual opt<size_t>         member_offset   (const std::string& struc, const std::string& member) = 0;
        virtual opt<Offset>         find_symbol     (size_t offset) = 0;
        virtual bool                list_symbols    (on_symbol_fn on_symbol) = 0;
    };

    struct Identity
    {
        std::string name;
        std::string id;
    };
    opt<Identity> identify_pdb(span_t span, const memory::Io& io);

    std::shared_ptr<Module> make_pdb    (const std::string& module, const std::string& guid);
    std::shared_ptr<Module> make_dwarf  (const std::string& module, const std::string& guid);
    std::shared_ptr<Module> make_map    (const std::string& module, const std::string& guid);

    struct Modules
    {
         Modules(core::Core& core);
        ~Modules();

        using on_module_fn = std::function<walk_e(span_t span, const Module& module)>;

        bool    insert  (proc_t proc, const std::string& module, span_t span, const std::shared_ptr<Module>& symbols);
        bool    insert  (proc_t proc, const memory::Io& io, span_t span);
        bool    remove  (proc_t proc, const std::string& module);

        bool            list            (proc_t proc, const on_module_fn& on_module);
        Module*         find            (proc_t proc, const std::string& module);
        opt<uint64_t>   address         (proc_t proc, const std::string& module, const std::string& symbol);
        void            struc_names     (proc_t proc, const std::string& module, const symbols::on_name_fn& on_struc);
        opt<size_t>     struc_size      (proc_t proc, const std::string& module, const std::string& struc);
        void            struc_members   (proc_t proc, const std::string& module, const std::string& struc, const symbols::on_name_fn& on_member);
        opt<uint64_t>   member_offset   (proc_t proc, const std::string& module, const std::string& struc, const std::string& member);
        Symbol          find            (proc_t proc, uint64_t addr);

        // remove me
        static Modules& modules(core::Core& core);

        struct Data;
        std::unique_ptr<Data> d_;
    };
} // namespace symbols
