//
//  store_provider.cpp
//  gbDemo
//
//  Created by serhii.s on 7/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "store_provider.h"
#import <StoreKit/StoreKit.h>

#if defined(__IOS__)

#import <GameAnalytics/GameAnalytics.h>

static NSString* k_no_ads_product_str_id = @"com.drift.hyper.no_ads";
static NSString* k_small_cash_pack_product_str_id = @"com.drift.hyper.small_cash_pack";
static NSString* k_medium_cash_pack_product_str_id = @"com.drift.hyper.medium_cash_pack";
static NSString* k_big_cash_pack_product_str_id = @"com.drift.hyper.big_cash_pack";
static NSString* k_vip_subscription_pack_product_str_id = @"com.drift.hyper.vip";

#else

static NSString* k_no_ads_product_str_id = @"com.drift.hyper.osx.no_ads";
static NSString* k_small_cash_pack_product_str_id = @"com.drift.hyper.osx.small_cash_pack";
static NSString* k_medium_cash_pack_product_str_id = @"com.drift.hyper.osx.medium_cash_pack";
static NSString* k_big_cash_pack_product_str_id = @"com.drift.hyper.osx.big_cash_pack";

#endif

@interface store_provider_impl : NSObject<SKPaymentTransactionObserver, SKProductsRequestDelegate>

+ (store_provider_impl* )shared_instance;
- (void)get_no_ads_product;
- (void)get_cash_pack_products;
- (void)get_vip_subscription_product;
- (void)buy_no_ads_product;
- (void)buy_small_cash_pack;
- (void)buy_medium_cash_pack;
- (void)buy_big_cash_pack;
- (void)buy_vip_subscription;
- (void)restore_products;
- (void)request_subscription_status;

@property (nonatomic, strong) SKPaymentQueue *m_default_queue;
@property (nonatomic, strong) SKProduct* m_no_ads_product;
@property (nonatomic, strong) SKProduct* m_small_cash_pack_product;
@property (nonatomic, strong) SKProduct* m_medium_cash_pack_product;
@property (nonatomic, strong) SKProduct* m_big_cash_pack_product;
@property (nonatomic, strong) SKProduct* m_vip_subscription_product;

@property (nonatomic) BOOL m_is_no_ads_product_bought;
@property (nonatomic) BOOL m_is_subsctiption_bought;

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
    });
    return instance;
}

- (instancetype)init
{
    self = [super init];
    if(self)
    {
        self.m_no_ads_product = nil;
        self.m_small_cash_pack_product = nil;
        self.m_medium_cash_pack_product = nil;
        self.m_big_cash_pack_product = nil;
        self.m_vip_subscription_product = nil;
        self.m_is_no_ads_product_bought = NO;
        self.m_is_subsctiption_bought = NO;
    }
    return self;
}

- (void)get_no_ads_product
{
    if ([SKPaymentQueue canMakePayments])
    {
        SKProductsRequest *request = [[SKProductsRequest alloc]
                                      initWithProductIdentifiers:
                                      [NSSet setWithObject:k_no_ads_product_str_id]];
        request.delegate = self;
        [request start];
    }
}

- (void)get_vip_subscription_product
{
    if ([SKPaymentQueue canMakePayments])
    {
        SKProductsRequest *request = [[SKProductsRequest alloc]
                                      initWithProductIdentifiers:
                                      [NSSet setWithObject:k_vip_subscription_pack_product_str_id]];
        request.delegate = self;
        [request start];
    }
}

- (void)get_cash_pack_products
{
    if ([SKPaymentQueue canMakePayments])
    {
        SKProductsRequest *request = [[SKProductsRequest alloc]
                                      initWithProductIdentifiers:
                                      [NSSet setWithObjects:k_small_cash_pack_product_str_id, k_medium_cash_pack_product_str_id, k_big_cash_pack_product_str_id, nil]];
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

- (void)buy_small_cash_pack
{
    if (self.m_small_cash_pack_product)
    {
        SKPayment *payment = [SKPayment paymentWithProduct:self.m_small_cash_pack_product];
        [self.m_default_queue addPayment:payment];
    }
    else
    {
        NSLog(@"can't handle payment. small cash pack product is null");
    }
}

- (void)buy_medium_cash_pack
{
    if (self.m_medium_cash_pack_product)
    {
        SKPayment *payment = [SKPayment paymentWithProduct:self.m_medium_cash_pack_product];
        [self.m_default_queue addPayment:payment];
    }
    else
    {
        NSLog(@"can't handle payment. medium cash pack product is null");
    }
}

- (void)buy_big_cash_pack
{
    if (self.m_big_cash_pack_product)
    {
        SKPayment *payment = [SKPayment paymentWithProduct:self.m_big_cash_pack_product];
        [self.m_default_queue addPayment:payment];
    }
    else
    {
        NSLog(@"can't handle payment. big cash pack product is null");
    }
}

- (void)buy_vip_subscription
{
    if (self.m_vip_subscription_product)
    {
        SKPayment *payment = [SKPayment paymentWithProduct:self.m_vip_subscription_product];
        [self.m_default_queue addPayment:payment];
    }
    else
    {
        NSLog(@"can't handle payment. vip subscription product is null");
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
            {
                [self.m_default_queue finishTransaction:transaction];
                NSString* product_identifier = [[transaction payment] productIdentifier];
                
                if ([product_identifier isEqualToString:k_no_ads_product_str_id])
                {
                    self.m_is_no_ads_product_bought = YES;
                    if (game::store_provider::shared_instance()->get_on_puchases_restored_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchases_restored_callback()(1);
                    }
                    
                    if (game::store_provider::shared_instance()->get_on_puchase_no_ads_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_no_ads_callback()(true);
                    }
                    
#if defined(__IOS__)
                    
                    [GameAnalytics addBusinessEventWithCurrency:@"USD" amount:1 itemType:@"functional" itemId:k_no_ads_product_str_id cartType:@"shop" autoFetchReceipt:YES];
                    
#endif
                   
                }
                if ([product_identifier isEqualToString:k_vip_subscription_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_vip_subscription_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_vip_subscription_callback()(true);
                    }
                }
                if ([product_identifier isEqualToString:k_small_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_small_cash_pack_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_small_cash_pack_callback()(true);
                    }
                    
#if defined(__IOS__)
                    
                    [GameAnalytics addBusinessEventWithCurrency:@"USD" amount:3000 itemType:@"currency" itemId:k_small_cash_pack_product_str_id cartType:@"shop" autoFetchReceipt:YES];
                    [GameAnalytics addResourceEventWithFlowType:GAResourceFlowTypeSource currency:@"cash" amount:@3000 itemType:@"IAP" itemId:k_small_cash_pack_product_str_id];
                    
#endif
                    
                }
                if ([product_identifier isEqualToString:k_medium_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_medium_cash_pack_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_medium_cash_pack_callback()(true);
                    }
                    
#if defined(__IOS__)
                    
                    [GameAnalytics addBusinessEventWithCurrency:@"USD" amount:10000 itemType:@"currency" itemId:k_medium_cash_pack_product_str_id cartType:@"shop" autoFetchReceipt:YES];
                    [GameAnalytics addResourceEventWithFlowType:GAResourceFlowTypeSource currency:@"cash" amount:@10000 itemType:@"IAP" itemId:k_medium_cash_pack_product_str_id];
                    
#endif
                    
                }
                if ([product_identifier isEqualToString:k_big_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_big_cash_pack_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_big_cash_pack_callback()(true);
                    }
                    
#if defined(__IOS__)
                    
                    [GameAnalytics addBusinessEventWithCurrency:@"USD" amount:20000 itemType:@"currency" itemId:k_big_cash_pack_product_str_id cartType:@"shop" autoFetchReceipt:YES];
                    [GameAnalytics addResourceEventWithFlowType:GAResourceFlowTypeSource currency:@"cash" amount:@20000 itemType:@"IAP" itemId:k_big_cash_pack_product_str_id];
                    
#endif
                    
                }
            }
            break;
            case SKPaymentTransactionStateFailed:
            {
                [self.m_default_queue finishTransaction:transaction];
                NSString* product_identifier = [[transaction payment] productIdentifier];
                if ([product_identifier isEqualToString:k_no_ads_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_no_ads_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_no_ads_callback()(false);
                    }
                }
                if ([product_identifier isEqualToString:k_vip_subscription_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_vip_subscription_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_vip_subscription_callback()(false);
                    }
                }
                if ([product_identifier isEqualToString:k_small_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_small_cash_pack_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_small_cash_pack_callback()(false);
                    }
                }
                if ([product_identifier isEqualToString:k_medium_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_medium_cash_pack_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_medium_cash_pack_callback()(false);
                    }
                }
                if ([product_identifier isEqualToString:k_big_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchase_big_cash_pack_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchase_big_cash_pack_callback()(false);
                    }
                    
                }
            }
            break;
            case SKPaymentTransactionStateRestored:
            {
                [self.m_default_queue finishTransaction:transaction];
                NSString* product_identifier = [[[transaction originalTransaction] payment] productIdentifier];
                if ([product_identifier isEqualToString:k_no_ads_product_str_id])
                {
                    self.m_is_no_ads_product_bought = YES;
                    if (game::store_provider::shared_instance()->get_on_puchases_restored_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchases_restored_callback()(1);
                    }
                }
                if ([product_identifier isEqualToString:k_small_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchases_restored_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchases_restored_callback()(2);
                    }
                }
                if ([product_identifier isEqualToString:k_medium_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchases_restored_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchases_restored_callback()(3);
                    }
                }
                if ([product_identifier isEqualToString:k_big_cash_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchases_restored_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchases_restored_callback()(4);
                    }
                }
                if ([product_identifier isEqualToString:k_vip_subscription_pack_product_str_id])
                {
                    if (game::store_provider::shared_instance()->get_on_puchases_restored_callback())
                    {
                        game::store_provider::shared_instance()->get_on_puchases_restored_callback()(5);
                    }
                    [self request_subscription_status];
                }
            }
            break;
            default:
            break;
        }
    }
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray *products = response.products;
    [products enumerateObjectsUsingBlock:^(SKProduct *product, NSUInteger idx, BOOL *stop) {
        if ([product.productIdentifier isEqualToString:k_no_ads_product_str_id])
        {
            self.m_no_ads_product = product;
        }
        
        if ([product.productIdentifier isEqualToString:k_small_cash_pack_product_str_id])
        {
            self.m_small_cash_pack_product = product;
        }
        
        if ([product.productIdentifier isEqualToString:k_medium_cash_pack_product_str_id])
        {
            self.m_medium_cash_pack_product = product;
        }
        
        if ([product.productIdentifier isEqualToString:k_big_cash_pack_product_str_id])
        {
            self.m_big_cash_pack_product = product;
        }
        
        if ([product.productIdentifier isEqualToString:k_vip_subscription_pack_product_str_id])
        {
            self.m_vip_subscription_product = product;
            [self request_subscription_status];
        }
        
        NSLog(@"product title = %@", product.localizedTitle);
        NSLog(@"product description = %@", product.localizedDescription);
    }];
}

- (void)request_subscription_status
{
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString* receipt_path = [NSBundle mainBundle].appStoreReceiptURL.path;
        if ([[NSFileManager defaultManager] fileExistsAtPath:receipt_path])
        {
            static dispatch_once_t once;
            dispatch_once(&once, ^{
                NSData* receipt_data = [NSData dataWithContentsOfURL:[NSBundle mainBundle].appStoreReceiptURL];
                NSDictionary* receipt_dictionary = @{@"receipt-data" : [receipt_data base64EncodedStringWithOptions:0],
                                                     @"password" : @"01aafbf50df54e118f5073bcc4b3f2d8"
                };
                NSError* error = nil;
                NSData* request_data = [NSJSONSerialization dataWithJSONObject:receipt_dictionary options:0 error:&error];
                
#if defined(DEBUG)
                
                NSURL* store_URL = [NSURL URLWithString: @"https://sandbox.itunes.apple.com/verifyReceipt"];
                
#else
                
                NSURL* store_URL = [NSURL URLWithString: @"https://buy.itunes.apple.com/verifyReceipt"];
                
#endif
                
                NSMutableURLRequest* store_request = [NSMutableURLRequest requestWithURL:store_URL];
                store_request.HTTPMethod = @"POST";
                store_request.HTTPBody = request_data;
                NSURLSession* session = [NSURLSession sessionWithConfiguration:NSURLSessionConfiguration.defaultSessionConfiguration];
                NSURLSessionDataTask* task = [session dataTaskWithRequest:store_request completionHandler:^(NSData* data, NSURLResponse* response, NSError* error) {
                    NSDictionary* json_response = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:&error];
                    
                    NSArray* receipt_info = json_response[@"latest_receipt_info"];
                    if (receipt_info)
                    {
                        NSDictionary* last_receipt = receipt_info.lastObject;
                        NSDateFormatter* formatter = [NSDateFormatter new];
                        formatter.dateFormat = @"yyyy-MM-dd HH:mm:ss VV";
                        NSDate* expiration_date = [formatter dateFromString:last_receipt[@"expires_date"]];
                        NSLog(@"%@", expiration_date);
                        
                        NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];
                        NSDateComponents *components = [calendar components:NSCalendarUnitMinute
                                                                   fromDate:[NSDate date]
                                                                     toDate:expiration_date
                                                                    options:0];
                        NSLog(@"%li", (long)components.minute);
                        if (components.minute > 0)
                        {
                            self.m_is_subsctiption_bought = YES;
                        }
                        else
                        {
                            self.m_is_subsctiption_bought = NO;
                        }
                        
                        if ([NSThread isMainThread])
                        {
                            if (game::store_provider::shared_instance()->get_on_subscription_status_changed_callback())
                            {
                                game::store_provider::shared_instance()->get_on_subscription_status_changed_callback()(self.m_is_subsctiption_bought);
                            }
                        }
                        else
                        {
                            dispatch_sync(dispatch_get_main_queue(),  ^{
                                if (game::store_provider::shared_instance()->get_on_subscription_status_changed_callback())
                                {
                                    game::store_provider::shared_instance()->get_on_subscription_status_changed_callback()(self.m_is_subsctiption_bought);
                                }
                            });
                        }
                    }
                }];
                [task resume];
            });
        }
        else
        {
            self.m_is_subsctiption_bought = NO;
            if (game::store_provider::shared_instance()->get_on_subscription_status_changed_callback())
            {
                game::store_provider::shared_instance()->get_on_subscription_status_changed_callback()(self.m_is_subsctiption_bought);
            }
        }
    });
    
}

@end

namespace game
    {
    const i32 store_provider::k_no_ads_product_id = 1;
    const i32 store_provider::k_cash_pack_1_product_id = 2;
    const i32 store_provider::k_cash_pack_2_product_id = 3;
    const i32 store_provider::k_cash_pack_3_product_id = 4;
    const i32 store_provider::k_vip_subscription_product_id = 5;
    
    std::shared_ptr<store_provider> store_provider::m_instance = nullptr;
    
    store_provider::store_provider()
    {
        
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
        [[store_provider_impl shared_instance] get_vip_subscription_product];
        
#endif
        
        [[store_provider_impl shared_instance] get_cash_pack_products];
        
    }
    
    void store_provider::buy_no_ads_product(const std::function<void(bool)>& callback)
    {
        
#if defined(__IOS__)
        m_on_purchase_no_ads = callback;
        [[store_provider_impl shared_instance] buy_no_ads_product];
        
#endif
        
    }
    
    void store_provider::set_on_puchases_restored_callback(const std::function<void(i32)>& callback)
    {
        m_on_purchases_restored = callback;
    }
    
    std::function<void(i32)> store_provider::get_on_puchases_restored_callback() const
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
        [[store_provider_impl shared_instance] restore_products];
    }
    
    void store_provider::buy_small_cash_pack(const std::function<void(bool)>& callback)
    {
        m_on_purchase_small_cash_pack = callback;
        [[store_provider_impl shared_instance] buy_small_cash_pack];
    }
    
    void store_provider::buy_medium_cash_pack(const std::function<void(bool)>& callback)
    {
        m_on_purchase_medium_cash_pack = callback;
        [[store_provider_impl shared_instance] buy_medium_cash_pack];
    }
    
    void store_provider::buy_big_cash_pack(const std::function<void(bool)>& callback)
    {
        m_on_purchase_big_cash_pack = callback;
        [[store_provider_impl shared_instance] buy_big_cash_pack];
    }
    
    void store_provider::buy_vip_subscription(const std::function<void(bool)>& callback)
    {
        m_on_purchase_vip_subscription = callback;
        [[store_provider_impl shared_instance] buy_vip_subscription];
    }
    
    std::function<void(bool)> store_provider::get_on_puchase_no_ads_callback() const
    {
        return m_on_purchase_no_ads;
    }
    
    std::function<void(bool)> store_provider::get_on_puchase_small_cash_pack_callback() const
    {
        return m_on_purchase_small_cash_pack;
    }
    
    std::function<void(bool)> store_provider::get_on_puchase_medium_cash_pack_callback() const
    {
        return m_on_purchase_medium_cash_pack;
    }
    
    std::function<void(bool)> store_provider::get_on_puchase_big_cash_pack_callback() const
    {
        return m_on_purchase_big_cash_pack;
    }

    std::function<void(bool)> store_provider::get_on_puchase_vip_subscription_callback() const
    {
        return m_on_purchase_vip_subscription;
    }
    
    void store_provider::set_on_subscription_status_changed_callback(const std::function<void(bool)>& callback)
    {
        m_on_subscription_status_changed = callback;
    }
    
    std::function<void(bool)> store_provider::get_on_subscription_status_changed_callback() const
    {
        return m_on_subscription_status_changed;
    }
}
    
