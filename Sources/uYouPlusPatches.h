#import <os/log.h>
#import <UIKit/UIActivityViewController.h>
#import <YouTubeHeader/YTUIUtils.h>
#import <YouTubeHeader/YTCommonUtils.h>
#import <YouTubeHeader/YTColorPalette.h>
#import <YouTubeHeader/YTCommonColorPalette.h>
#import "Tweaks/protobuf/objectivec/GPBDescriptor.h"
#import "Tweaks/protobuf/objectivec/GPBUnknownField.h"
#import "Tweaks/protobuf/objectivec/GPBUnknownFieldSet.h"
#import "uYouPlus.h"

// http3 protocol (iOS 15+)
@interface NetworkManager : NSObject <NSURLSessionDataDelegate>
- (void)triggerHTTP3Request;
@end

@implementation NetworkManager
- (void)triggerHTTP3Request {
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
    config.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
    config.HTTPShouldUsePipelining = YES;
    config.HTTPShouldSetCookies = YES;
    
    NSURLSession *session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:[NSOperationQueue mainQueue]];
    
    NSURL *url = [NSURL URLWithString:@"https://google.com"];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url cachePolicy:NSURLRequestReloadIgnoringLocalCacheData timeoutInterval:60.0];
    request.HTTPShouldHandleCookies = YES;
    request.assumesHTTP3Capable = YES;
    
    os_log("Task will start, url: %@", url.absoluteString);
    
    [[session dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        if (error) {
            os_log("Task transport error %@", error.localizedDescription);
            return;
        }
        
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
        
        if (httpResponse.statusCode >= 200 && httpResponse.statusCode < 300) {
            os_log("Task finished with status %ld, bytes %lu", (long)httpResponse.statusCode, (unsigned long)data.length);
        } else {
            NSLog(@"Task response status code is invalid; received %ld, but expected 2xx", (long)httpResponse.statusCode);
        }
        
    }] resume];
}
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didFinishCollectingMetrics:(NSURLSessionTaskMetrics *)metrics {
    NSArray *protocols = [metrics.transactionMetrics valueForKeyPath:@"networkProtocolName"];
    os_log("Protocols: %@", protocols);
}
@end

@interface PlayerManager : NSObject
// Prevent uYou player bar from showing when not playing downloaded media
- (float)progress;
// Prevent uYou's playback from colliding with YouTube's
- (void)setSource:(id)source;
- (void)pause;
+ (id)sharedInstance;
@end

// iOS 16 uYou crash fix - @level3tjg: https://github.com/qnblackcat/uYouPlus/pull/224
@interface OBPrivacyLinkButton : UIButton
- (instancetype)initWithCaption:(NSString *)caption
                     buttonText:(NSString *)buttonText
                          image:(UIImage *)image
                      imageSize:(CGSize)imageSize
                   useLargeIcon:(BOOL)useLargeIcon
                displayLanguage:(NSString *)displayLanguage;
@end

// uYouLocal fix
// @interface YTLocalPlaybackController : NSObject
// - (id)activeVideo;
// @end

// uYou theme fix
// @interface YTAppDelegate ()
// @property(nonatomic, strong) id downloadsVC;
// @end

// Fix uYou's appearance not updating if the app is backgrounded
@interface DownloadsPagerVC : UIViewController
- (NSArray<UIViewController *> *)viewControllers;
- (void)updatePageStyles;
@end
@interface DownloadingVC : UIViewController
- (void)updatePageStyles;
- (UITableView *)tableView;
@end
@interface DownloadingCell : UITableViewCell
- (void)updatePageStyles;
@end
@interface DownloadedVC : UIViewController
- (void)updatePageStyles;
- (UITableView *)tableView;
@end
@interface DownloadedCell : UITableViewCell
- (void)updatePageStyles;
@end
@interface UILabel (uYou)
+ (id)_defaultColor;
@end

// YouTube Native Share Headers - https://github.com/jkhsjdhjs/youtube-native-share - @jkhsjdhjs
@interface CustomGPBMessage : GPBMessage
+ (instancetype)deserializeFromString:(NSString*)string;
@end

// @interface YTICommand : GPBMessage
// @end

@interface ELMPBCommand : GPBMessage
@end

@interface ELMPBShowActionSheetCommand : GPBMessage
@property (nonatomic, strong, readwrite) ELMPBCommand *onAppear;
@property (nonatomic, assign, readwrite) BOOL hasOnAppear;
@end

@interface ELMContext : NSObject
@property (nonatomic, strong, readwrite) UIView *fromView;
@end

@interface ELMCommandContext : NSObject
@property (nonatomic, strong, readwrite) ELMContext *context;
@end

@interface YTIUpdateShareSheetCommand
@property (nonatomic, assign, readwrite) BOOL hasSerializedShareEntity;
@property (nonatomic, copy, readwrite) NSString *serializedShareEntity;
+ (GPBExtensionDescriptor*)updateShareSheetCommand;
@end

@interface YTIInnertubeCommandExtensionRoot
+ (GPBExtensionDescriptor*)innertubeCommand;
@end

@interface YTAccountScopedCommandResponderEvent
@property (nonatomic, strong, readwrite) YTICommand *command;
@property (nonatomic, strong, readwrite) UIView *fromView;
@end

@interface YTIShareEntityEndpoint
@property (nonatomic, assign, readwrite) BOOL hasSerializedShareEntity;
@property (nonatomic, copy, readwrite) NSString *serializedShareEntity;
+ (GPBExtensionDescriptor*)shareEntityEndpoint;
@end
