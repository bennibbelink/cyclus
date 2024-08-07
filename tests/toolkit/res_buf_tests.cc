#include "res_buf_tests.h"
#include "toolkit/mat_query.h"

#include <gtest/gtest.h>

namespace cyclus {
namespace toolkit {

// The "Empty" suffix indicates the test uses the store_ instance of
// ResBuf. The "Filled" suffix indicates the test uses the
// filled_store_ instance of ResBuf.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - Getters, Setters, and Property changers - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, set_capacity_ExceptionsEmpty) {
  EXPECT_THROW(store_.capacity(neg_cap), ValueError);
  EXPECT_NO_THROW(store_.capacity(zero_cap));
  EXPECT_NO_THROW(store_.capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, set_capacity_ExceptionsFilled) {
  EXPECT_THROW(filled_store_.capacity(low_cap), ValueError);
  EXPECT_NO_THROW(filled_store_.capacity(cap));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetCapacity_ExceptionsEmpty) {
  ASSERT_NO_THROW(store_.capacity());
  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.capacity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetCapacity_InitialEmpty) {
  EXPECT_DOUBLE_EQ(store_.capacity(), INFINITY);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, Getset_capacityEmpty) {
  store_.capacity(zero_cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), zero_cap);

  store_.capacity(cap);
  EXPECT_DOUBLE_EQ(store_.capacity(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetSpace_Empty) {
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), INFINITY);

  store_.capacity(zero_cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), zero_cap);

  store_.capacity(cap);
  ASSERT_NO_THROW(store_.space());
  EXPECT_DOUBLE_EQ(store_.space(), cap);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetSpace_Filled) {
  double space = cap - (mat1_->quantity() + mat2_->quantity());
  ASSERT_NO_THROW(filled_store_.space());
  EXPECT_DOUBLE_EQ(filled_store_.space(), space);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetQuantity_Empty) {
  ASSERT_NO_THROW(store_.quantity());
  EXPECT_DOUBLE_EQ(store_.quantity(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetQuantity_Filled) {
  ASSERT_NO_THROW(filled_store_.quantity());
  double quantity = mat1_->quantity() + mat2_->quantity();
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), quantity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetCount_Empty) {
  ASSERT_NO_THROW(store_.count());
  EXPECT_EQ(store_.count(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, GetCount_Filled) {
  ASSERT_NO_THROW(filled_store_.count());
  EXPECT_DOUBLE_EQ(filled_store_.count(), 2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Removing from buffer  - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveQty_ExceptionsEmpty) {
  Product::Ptr p;
  double qty = cap + overeps;
  ASSERT_THROW(p = filled_store_.Pop(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveQty_ExceptionsFilled) {
  Product::Ptr p;
  double qty = cap + overeps;
  ASSERT_THROW(p = store_.Pop(qty), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveQty_SingleNoSplit) {
  // pop one no splitting leaving one mat in the store
  Product::Ptr p;

  ASSERT_NO_THROW(p = filled_store_.Pop(exact_qty));
  EXPECT_EQ(p, mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveQty_SingleWithSplit) {
  // pop one no splitting leaving one mat in the store

  Product::Ptr p;

  double orig_qty = filled_store_.quantity();
  ASSERT_NO_THROW(p = filled_store_.Pop(exact_qty_under));
  EXPECT_DOUBLE_EQ(p->quantity(), exact_qty_under);
  EXPECT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), orig_qty - exact_qty_under);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveQty_DoubleWithSplit) {
  // pop one no splitting leaving one mat in the store

  Product::Ptr p;

  double orig_qty = filled_store_.quantity();
  ASSERT_NO_THROW(p = filled_store_.Pop(exact_qty_over));
  EXPECT_DOUBLE_EQ(p->quantity(), exact_qty_over);
  EXPECT_TRUE(filled_store_.count() == 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), orig_qty - exact_qty_over);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveNum_ExceptionsFilled) {
  ProdVec manifest;
  ASSERT_THROW(manifest = filled_store_.PopN(3), ValueError);
  ASSERT_THROW(manifest = filled_store_.PopN(-1), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveNum_ZeroFilled) {
  ProdVec manifest;
  double tot_qty = filled_store_.quantity();

  ASSERT_NO_THROW(manifest = filled_store_.PopN(0));
  ASSERT_EQ(manifest.size(), 0);
  ASSERT_EQ(filled_store_.count(), 2);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), tot_qty);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveNum_OneFilled) {
  ProdVec manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopN(1));
  ASSERT_EQ(manifest.size(), 1);
  ASSERT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveNum_TwoFilled) {
  ProdVec manifest;

  ASSERT_NO_THROW(manifest = filled_store_.PopN(2));
  ASSERT_EQ(manifest.size(), 2);
  ASSERT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(manifest.at(0)->quantity(), mat1_->quantity());
  EXPECT_EQ(manifest.at(0), mat1_);
  EXPECT_DOUBLE_EQ(manifest.at(1)->quantity(), mat2_->quantity());
  EXPECT_EQ(manifest.at(1), mat2_);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, RemoveOne_Filled) {
  Product::Ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.Pop());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat2_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.Pop());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);

  ASSERT_THROW(mat = filled_store_.Pop(), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, PopBack) {
  Product::Ptr mat;

  ASSERT_NO_THROW(mat = filled_store_.PopBack());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat2_->quantity());
  EXPECT_EQ(mat, mat2_);
  EXPECT_EQ(filled_store_.count(), 1);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(mat = filled_store_.PopBack());
  EXPECT_DOUBLE_EQ(mat->quantity(), mat1_->quantity());
  EXPECT_EQ(mat, mat1_);
  EXPECT_EQ(filled_store_.count(), 0);
  EXPECT_DOUBLE_EQ(filled_store_.quantity(), 0.0);

  ASSERT_THROW(mat = filled_store_.PopBack(), ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - Pushing into buffer - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, Push_Empty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());

  ASSERT_NO_THROW(store_.Push(mat2_));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, Push_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_NO_THROW(store_.Push(mat2_));

  double topush = cap - store_.quantity();
  Product::Ptr overmat = Product::CreateUntracked(topush + overeps, "food");

  ASSERT_THROW(store_.Push(overmat), ValueError);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmat = Product::CreateUntracked(topush + undereps, "food");
  ASSERT_NO_THROW(store_.Push(overmat));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, Push_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));

  ASSERT_NO_THROW(store_.Push(mat1_));
  ASSERT_THROW(store_.Push(mat1_), KeyError);

  ASSERT_EQ(store_.count(), 1);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, PushAll_Empty) {
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(mats));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, PushAll_ResCast) {
  ResVec rs;
  for (int i = 0; i < mats.size(); ++i) {
    rs.push_back(mats[i]);
  }
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(rs));
  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, PushAll_NoneEmpty) {
  ProdVec manifest;
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(manifest));
  ASSERT_EQ(store_.count(), 0);
  EXPECT_DOUBLE_EQ(store_.quantity(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, PushAll_RetrieveOrderEmpty) {
  Product::Ptr mat;

  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(mats));
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat1_);
  ASSERT_NO_THROW(mat = store_.Pop());
  ASSERT_EQ(mat, mat2_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, PushAll_OverCapacityEmpty) {
  ASSERT_NO_THROW(store_.capacity(cap));
  ASSERT_NO_THROW(store_.Push(mats));

  double topush = cap - store_.quantity();
  Product::Ptr overmat = Product::CreateUntracked(topush + overeps, "food");
  ProdVec overmats;
  overmats.push_back(overmat);

  ASSERT_THROW(store_.Push(overmats), ValueError);
  ASSERT_EQ(store_.count(), 2);
  ASSERT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());

  overmats.clear();
  overmat = Product::CreateUntracked(topush + undereps, "food");
  overmats.push_back(overmat);

  ASSERT_NO_THROW(store_.Push(overmats));
  ASSERT_EQ(store_.count(), 3);

  double expected = mat1_->quantity() + mat2_->quantity() + overmat->quantity();
  ASSERT_DOUBLE_EQ(store_.quantity(), expected);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ProductBufTest, PushAll_DuplicateEmpty) {
  ASSERT_NO_THROW(store_.capacity(2 * cap));

  ASSERT_NO_THROW(store_.Push(mats));
  ASSERT_THROW(store_.Push(mat1_), KeyError);
  ASSERT_THROW(store_.Push(mat2_), KeyError);
  ASSERT_THROW(store_.Push(mats), KeyError);

  ASSERT_EQ(store_.count(), 2);
  EXPECT_DOUBLE_EQ(store_.quantity(), mat1_->quantity() + mat2_->quantity());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Special tests for material buffers
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TEST_F(MaterialBufTest, NonBulkTest) {

  ASSERT_EQ(mat_store_.count(), 0);
  EXPECT_DOUBLE_EQ(mat_store_.capacity(), cap_);

  mat_store_.Push(mat1a_);
  mat_store_.Push(mat2a_);

  ASSERT_EQ(mat_store_.count(), 2);
  EXPECT_DOUBLE_EQ(mat_store_.capacity(), cap_);
  double qty_exp = mat1a_->quantity() + mat2a_->quantity();
  double space_exp = cap_ - qty_exp;
  EXPECT_DOUBLE_EQ(mat_store_.space(), space_exp);
  EXPECT_DOUBLE_EQ(mat_store_.quantity(), qty_exp);

  Material::Ptr pop1_ = mat_store_.Pop();
  ASSERT_EQ(pop1_->comp(), test_comp1_);
  Material::Ptr pop2_ = mat_store_.Pop();
  ASSERT_EQ(pop2_->comp(), test_comp2_);

}

TEST_F(MaterialBufTest, BulkTest) {

  ASSERT_EQ(bulk_store_.count(), 0);
  EXPECT_DOUBLE_EQ(bulk_store_.capacity(), cap_);

  bulk_store_.Push(mat1b_);
  bulk_store_.Push(mat2b_);

  ASSERT_EQ(bulk_store_.count(), 1);
  EXPECT_DOUBLE_EQ(bulk_store_.capacity(), cap_);
  double qty_exp = mat1b_->quantity() + mat2b_->quantity();
  double space_exp = cap_ - qty_exp;
  EXPECT_DOUBLE_EQ(bulk_store_.space(), space_exp);
  EXPECT_DOUBLE_EQ(bulk_store_.quantity(), qty_exp);

  Material::Ptr pop1_ = bulk_store_.Pop();
  cyclus::toolkit::MatQuery mq1(pop1_);
  double sr89_qty = mq1.mass(sr89_);
  double fe59_qty = mq1.mass(fe59_);

  double sr89_qty_exp = 5.0 * units::g;
  double fe59_qty_exp = 5.0 * units::g;

  EXPECT_DOUBLE_EQ(sr89_qty_exp, sr89_qty);  
  EXPECT_DOUBLE_EQ(fe59_qty_exp, fe59_qty);  

  ASSERT_EQ(bulk_store_.count(), 0);
  EXPECT_DOUBLE_EQ(bulk_store_.capacity(), cap_);
  EXPECT_DOUBLE_EQ(bulk_store_.space(), cap_);
  EXPECT_DOUBLE_EQ(bulk_store_.quantity(), 0);
  
  EXPECT_DOUBLE_EQ(pop1_->quantity(), qty_exp);

  bulk_store_.Push(pop1_);
  // need to capture this now because mat3_ is about to disappear
  qty_exp += mat3_->quantity();
  space_exp -= mat3_->quantity();
  sr89_qty_exp += mat3_->quantity()/3;
  fe59_qty_exp += mat3_->quantity()*2/3;

  bulk_store_.Push(mat3_);

  ASSERT_EQ(bulk_store_.count(), 1);
  EXPECT_DOUBLE_EQ(bulk_store_.capacity(), cap_);
  EXPECT_DOUBLE_EQ(qty_exp, bulk_store_.quantity());
  EXPECT_DOUBLE_EQ(space_exp, bulk_store_.space());

  pop1_ = bulk_store_.Pop();

  sr89_qty = mq1.mass(sr89_);
  fe59_qty = mq1.mass(fe59_);

  EXPECT_DOUBLE_EQ(sr89_qty_exp, sr89_qty);  
  EXPECT_DOUBLE_EQ(fe59_qty_exp, fe59_qty);  

}

TEST_F(MaterialBufTest, KeepPackaging) {
  keep_pkg_store_.Push(mat4_pkgd_);
  ASSERT_EQ(keep_pkg_store_.Pop()->package_name(), pkg_name_);
}

TEST_F(MaterialBufTest, ChangeKeepPackaging) {
  keep_pkg_store_.Push(mat4_pkgd_);
  ASSERT_EQ(keep_pkg_store_.Pop()->package_name(), pkg_name_);

  keep_pkg_store_.keep_packaging(false);
  keep_pkg_store_.Push(mat4_pkgd_);
  ASSERT_EQ(keep_pkg_store_.Pop()->package_name(), cyclus::Package::unpackaged_name());
}

TEST_F(MaterialBufTest, BulkKeepPackaging) {
  EXPECT_THROW(ResBuf<Material>(true, true), ValueError);
}


}  // namespace toolkit
}  // namespace cyclus
