//
//  store_provider.cpp
//  gbDemo
//
//  Created by serhii.s on 7/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "store_provider.h"

#if defined(__IOS__)

#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

static NSString* k_no_ads_product_id = @"com.drift.hyper.no_ads";

@interface store_provider_impl : NSObject<SKPaymentTransactionObserver, SKProductsRequestDelegate>

+ (store_provider_impl* )shared_instance;
- (void)get_no_ads_product;
- (void)buy_no_ads_product;
- (void)restore_products;

@property (nonatomic, strong) SKPaymentQueue *m_default_queue;
@property (nonatomic, strong) SKProduct* m_no_ads_product;

@property (nonatomic) BOOL m_is_no_ads_product_bought;

@end

@implementation store_provider_impl

+ (store_provider_impl* )shared_instance
{
    static store_provider_impl *instance = nil;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        instance = [[self alloc] init];
        instance.m_default_queue = [SKPaymentQueue defaultQueue];
        [instance.m_default_queue addTransactionObserver:instance];
        
        /* if ([SKPaymentQueue canMakePayments])
        {
            [instance.m_default_queue restoreCompletedTransactions];
        }*/
    });
    return instance;
}

- (instancetype)init
{
    self = [super init];
    if(self)
    {
        self.m_no_ads_product = nil;
        self.m_is_no_ads_product_bought = NO;
    }
    return self;
}

- (void)get_no_ads_product
{
    if ([SKPaymentQueue canMakePayments])
    {
        SKProductsRequest *request = [[SKProductsRequest alloc]
                                      initWithProductIdentifiers:
                                      [NSSet setWithObject:k_no_ads_product_id]];
        request.delegate = self;
        [request start];
    }
}

- (void)buy_no_ads_product
{
    if (self.m_no_ads_product)
    {
        SKPayment *payment = [SKPayment paymentWithProduct:self.m_no_ads_product];
        [self.m_default_queue addPayment:payment];
    }
    else
    {
        NSLog(@"can't handle payment. no ads product is null");
    }
}

- (void)restore_products
{
    [self.m_default_queue restoreCompletedTransactions];
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self.m_default_queue finishTransaction:transaction];
                if ([[[[transaction originalTransaction] payment] productIdentifier] isEqualToString:k_no_ads_product_id])
                {
                    self.m_is_no_ads_product_bought = YES;
                }
            break;
            case SKPaymentTransactionStateFailed:
                [self.m_default_queue finishTransaction:transaction];
            break;
            case SKPaymentTransactionStateRestored:
                [self.m_default_queue finishTransaction:transaction];
                if ([[[[transaction originalTransaction] payment] productIdentifier] isEqualToString:k_no_ads_product_id])
                {
                    self.m_is_no_ads_product_bought = YES;
                }
            break;
            default:
            break;
        }
    }
    
    if (game::store_provider::shared_instance()->get_on_puchases_restored_callback())
    {
        game::store_provider::shared_instance()->get_on_puchases_restored_callback()();
    }
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray *products = response.products;
    if ([products count] != 0)
    {
        self.m_no_ads_product = [products objectAtIndex:0];
        NSLog(@"product title = %@", self.m_no_ads_product.localizedTitle);
        NSLog(@"product description = %@", self.m_no_ads_product.localizedDescription);
    }
}

@end

#endif

namespace game
{
    const i32 store_provider::k_no_ads_product_id = 1;
    const i32 store_provider::k_cash_pack_1_product_id = 2;
    const i32 store_provider::k_cash_pack_2_product_id = 3;
    const i32 store_provider::k_cash_pack_3_product_id = 4;
    
    std::shared_ptr<store_provider> store_provider::m_instance = nullptr;
    
    store_provider::store_provider()
    {
        
#if defined(__IOS__)
        
        
        
#endif
        
    }
    
    store_provider::~store_provider()
    {
        
    }
    
    std::shared_ptr<store_provider> store_provider::shared_instance()
    {
        if(!m_instance)
        {
            m_instance = std::make_shared<store_provider>();
        }
        return m_instance;
    }
    
    void store_provider::request_products()
    {
        
#if defined(__IOS__)
        
        [[store_provider_impl shared_instance] get_no_ads_product];
        
#endif
        
    }
    
    void store_provider::buy_no_ads_product()
    {
        
#if defined(__IOS__)
        
        [[store_provider_impl shared_instance] buy_no_ads_product];
        
#endif
        
    }
    
    void store_provider::set_on_puchases_restored_callback(const std::function<void()>& callback)
    {
        m_on_purchases_restored = callback;
    }
    
    std::function<void()> store_provider::get_on_puchases_restored_callback() const
    {
        return m_on_purchases_restored;
    }
    
    bool store_provider::is_no_ads_product_bought() const
    {
#if defined(__IOS__)
        
        return [store_provider_impl shared_instance].m_is_no_ads_product_bought;
        
#endif
        
        return true;
    }
    
    void store_provider::restore_products()
    {
#if defined(__IOS__)
        
        [[store_provider_impl shared_instance] restore_products];
        
#endif
    }
}
    
