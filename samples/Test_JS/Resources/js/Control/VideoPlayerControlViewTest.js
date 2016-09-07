/**
 * Created by zhanglei on 16/8/4.
 */
var VideoPlayerControlViewTest = ca.CAViewController.extend({
    ctor: function () {
        this._super();

        //// Do any additional setup after loading the view from its nib.
        //var VideoPlayerControlView = ca.CAVideoPlayerControlView.createWithLayout(DLayout(DLayoutFill));
        //VideoPlayerControlView.setUrl("http://42.62.6.159/big_buck_bunny.mp4");
        ////VideoPlayerControlView->setFullPath(CCFileUtils.sharedFileUtils()->fullPathForFilename("image/abc.mp4"));
        //VideoPlayerControlView.setTitle("视屏");
        ////VideoPlayerControlView->setShowBackButton(false);
        //VideoPlayerControlView.setPlayerControlViewDelegate(this);
        //this.getView().addSubview(VideoPlayerControlView);

        var lab = ca.CALabel.createWithFrame(ca.rect(200,400, 400, 400));
        lab.setFontSize(40);
        lab.setText("此版本占时不能调用！");
        this.getView().addSubview(lab);

    },
    btncallback: function () {
        log("btncallback-->");
    }
});