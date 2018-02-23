#include <scorum/protocol/committee_proposal_operations.hpp>

#include <scorum/protocol/operation_util_impl.hpp>

#include <fc/static_variant.hpp>

// namespace fc {

// void to_variant(const scorum::protocol::proposal_operations& var, fc::variant& vo)
//{
//    var.visit(from_operation(vo));
//}

// void from_variant(const fc::variant& var, scorum::protocol::proposal_operations& vo)
//{
//    static std::map<std::string, uint32_t> to_tag = []() {
//        std::map<std::string, uint32_t> name_map;
//        for (int i = 0; i < scorum::protocol::proposal_operations::count(); ++i)
//        {
//            scorum::protocol::proposal_operations tmp;
//            tmp.set_which(i);
//            std::string n;
//            tmp.visit(get_operation_name(n));
//            name_map[n] = i;
//        }
//        return name_map;
//    }();

//    auto ar = var.get_array();
//    if (ar.size() < 2)
//        return;
//    if (ar[0].is_uint64())
//        vo.set_which(ar[0].as_uint64());
//    else
//    {
//        auto itr = to_tag.find(ar[0].as_string());
//        FC_ASSERT(itr != to_tag.end(), "Invalid operation name: ${n}", ("n", ar[0]));
//        vo.set_which(to_tag[ar[0].as_string()]);
//    }
//    vo.visit(fc::to_static_variant(ar[1]));
//}

//} // namespace fc
