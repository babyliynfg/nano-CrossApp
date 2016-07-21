#ifndef __ACTION_CCEASE_ACTION_H__
#define __ACTION_CCEASE_ACTION_H__

#include "game/actions/CGActionInterval.h"

NS_CC_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/** 
 @class ActionEase
 @brief Base class for Easing actions.
 @details Ease actions are created from other interval actions.
         The ease action will change the timeline of the inner action.
 @ingroup Actions
 */
class CC_DLL ActionEase : public ActionInterval
{
public:

    /**
    @brief Get the pointer of the inner action.
    @return The pointer of the inner action.
    */
    virtual ActionInterval* getInnerAction();

    //
    // Overrides
    //
    virtual ActionEase* copy()
    {
        CC_ASSERT(0);
        return nullptr;
    }
    
    virtual ActionEase* reverse() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }

    virtual void startWithTarget(CGNode *target) override;
    virtual void stop() override;
    virtual void update(float time) override;

public:
    ActionEase() {}
    virtual ~ActionEase();
    /** 
     @brief Initializes the action.
     @return Return true when the initialization success, otherwise return false.
    */
    bool initWithAction(ActionInterval *action);

protected:
    /** The inner action */
    ActionInterval *_inner;
private:
    M_DISALLOW_COPY_AND_ASSIGN(ActionEase);
};

/** 
 @class EaseRateAction
 @brief Base class for Easing actions with rate parameters.
 @details Ease the inner action with specified rate.
 @ingroup Actions
 */
class CC_DLL EaseRateAction : public ActionEase
{
public:
    /**
     @brief Creates the action with the inner action and the rate parameter.
     @param action A given ActionInterval
     @param rate A given rate
     @return An autoreleased EaseRateAction object.
    **/
    static EaseRateAction* create(ActionInterval* action, float rate);
    
    /**
     @brief Set the rate value for the ease rate action.
     @param rate The value will be set.
     */
    inline void setRate(float rate) { _rate = rate; }
    /**
     @brief Get the rate value of the ease rate action.
     @return Return the rate value of the ease rate action.
     */
    inline float getRate() const { return _rate; }

    //
    // Overrides
    //
    virtual EaseRateAction* copy()
    {
        CC_ASSERT(0);
        return nullptr;
    }
    virtual EaseRateAction* reverse() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }

public:
    EaseRateAction() {}
    virtual ~EaseRateAction();
    /** 
     @brief Initializes the action with the inner action and the rate parameter.
     @param pAction The pointer of the inner action.
     @param fRate The value of the rate parameter.
     @return Return true when the initialization success, otherwise return false.
     */
    bool initWithAction(ActionInterval *pAction, float fRate);

protected:
    float _rate;

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseRateAction);
};

/** 
 @class EaseIn
 @brief EaseIn action with a rate.
 @details The timeline of inner action will be changed by:
         \f${ time }^{ rate }\f$.
 @ingroup Actions
 */
class CC_DLL EaseIn : public EaseRateAction
{
public:
    /** 
     @brief Create the action with the inner action and the rate parameter.
     @param action The pointer of the inner action.
     @param rate The value of the rate parameter.
     @return A pointer of EaseIn action. If creation failed, return nil.
    */
    static EaseIn* create(ActionInterval* action, float rate);

    // Overrides
    virtual void update(float time) override;
    virtual EaseIn* copy();
    virtual EaseIn* reverse() const override;

public:
    EaseIn() {}
    virtual ~EaseIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseIn);
};

/** 
 @class EaseOut
 @brief EaseOut action with a rate.
 @details The timeline of inner action will be changed by:
         \f${ time }^ { (1/rate) }\f$.
 @ingroup Actions
 */
class CC_DLL EaseOut : public EaseRateAction
{
public:
    /** 
     @brief Create the action with the inner action and the rate parameter.
     @param action The pointer of the inner action.
     @param rate The value of the rate parameter.
     @return A pointer of EaseOut action. If creation failed, return nil.
    */
    static EaseOut* create(ActionInterval* action, float rate);

    // Overrides
    virtual void update(float time) override;
    virtual EaseOut* copy()  override;
    virtual EaseOut* reverse() const  override;

public:
    EaseOut() {}
    virtual ~EaseOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseOut);
};

/** 
 @class EaseInOut
 @brief EaseInOut action with a rate
 @details If time * 2 < 1, the timeline of inner action will be changed by:
         \f$0.5*{ time }^{ rate }\f$.
         Else, the timeline of inner action will be changed by:
         \f$1.0-0.5*{ 2-time }^{ rate }\f$.
 @ingroup Actions
 */
class CC_DLL EaseInOut : public EaseRateAction
{
public:
    /** 
     @brief Create the action with the inner action and the rate parameter.
     @param action The pointer of the inner action.
     @param rate The value of the rate parameter.
     @return A pointer of EaseInOut action. If creation failed, return nil.
    */
    static EaseInOut* create(ActionInterval* action, float rate);

    // Overrides
    virtual void update(float time) override;
    virtual EaseInOut* copy()  override;
    virtual EaseInOut* reverse() const  override;

public:
    EaseInOut() {}
    virtual ~EaseInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseInOut);
};

/** 
 @class EaseExponentialIn
 @brief Ease Exponential In action.
 @details The timeline of inner action will be changed by:
         \f${ 2 }^{ 10*(time-1) }-1*0.001\f$.
 @ingroup Actions
 */
class CC_DLL EaseExponentialIn : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseExponentialIn action. If creation failed, return nil.
    */
    static EaseExponentialIn* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseExponentialIn* copy();
    virtual ActionEase* reverse() const override;

public:
    EaseExponentialIn() {}
    virtual ~EaseExponentialIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseExponentialIn);
};

/** 
 @class EaseExponentialOut
 @brief Ease Exponential Out
 @details The timeline of inner action will be changed by:
         \f$1-{ 2 }^{ -10*(time-1) }\f$.
 @ingroup Actions
 */
class CC_DLL EaseExponentialOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseExponentialOut action. If creation failed, return nil.
    */
    static EaseExponentialOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseExponentialOut* copy();
    virtual ActionEase* reverse() const override;

public:
    EaseExponentialOut() {}
    virtual ~EaseExponentialOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseExponentialOut);
};

/** 
 @class EaseExponentialInOut
 @brief Ease Exponential InOut
 @details If time * 2 < 1, the timeline of inner action will be changed by:
         \f$0.5*{ 2 }^{ 10*(time-1) }\f$.
         else, the timeline of inner action will be changed by:
         \f$0.5*(2-{ 2 }^{ -10*(time-1) })\f$.
 @ingroup Actions
 */
class CC_DLL EaseExponentialInOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseExponentialInOut action. If creation failed, return nil.
    */
    static EaseExponentialInOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseExponentialInOut* copy();
    virtual EaseExponentialInOut* reverse() const override;

public:
    EaseExponentialInOut() {}
    virtual ~EaseExponentialInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseExponentialInOut);
};

/** 
 @class EaseSineIn
 @brief Ease Sine In
 @details The timeline of inner action will be changed by:
         \f$1-cos(time*\frac { \pi  }{ 2 } )\f$.
 @ingroup Actions
 */
class CC_DLL EaseSineIn : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseSineIn action. If creation failed, return nil.
    */
    static EaseSineIn* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseSineIn* copy();
    virtual ActionEase* reverse() const override;

public:
    EaseSineIn() {}
    virtual ~EaseSineIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseSineIn);
};

/** 
 @class EaseSineOut
 @brief Ease Sine Out
 @details The timeline of inner action will be changed by:
         \f$sin(time*\frac { \pi  }{ 2 } )\f$.
 @ingroup Actions
 */
class CC_DLL EaseSineOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseSineOut action. If creation failed, return nil.
    */
    static EaseSineOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseSineOut* copy();
    virtual ActionEase* reverse() const override;

public:
    EaseSineOut() {}
    virtual ~EaseSineOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseSineOut);
};

/** 
 @class EaseSineInOut
 @brief Ease Sine InOut
 @details The timeline of inner action will be changed by:
         \f$-0.5*(cos(\pi *time)-1)\f$.
 @ingroup Actions
 */
class CC_DLL EaseSineInOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseSineInOut action. If creation failed, return nil.
    */
    static EaseSineInOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseSineInOut* copy();
    virtual EaseSineInOut* reverse() const override;

public:
    EaseSineInOut() {}
    virtual ~EaseSineInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseSineInOut);
};

/** 
 @class EaseElastic
 @brief Ease Elastic abstract class
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseElastic : public ActionEase
{
public:

    /** 
     @brief Get period of the wave in radians. Default value is 0.3.
     @return Return the period of the wave in radians.
    */
    inline float getPeriod() const { return _period; }
    /**
     @brief Set period of the wave in radians.
     @param fPeriod The value will be set.
    */
    inline void setPeriod(float fPeriod) { _period = fPeriod; }

    //
    // Overrides
    //
    virtual EaseElastic* copy()
    {
        CC_ASSERT(0);
        return nullptr;
    }
    
    virtual EaseElastic* reverse() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }

public:
    EaseElastic() {}
    virtual ~EaseElastic() {}
    /** 
     @brief Initializes the action with the inner action and the period in radians.
     @param action The pointer of the inner action.
     @param period Period of the wave in radians. Default is 0.3.
     @return Return true when the initialization success, otherwise return false.
    */
    bool initWithAction(ActionInterval *action, float period = 0.3f);

protected:
    float _period;

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseElastic);

};

/** 
 @class EaseElasticIn
 @brief Ease Elastic In action.
 @details If time == 0 or time == 1, the timeline of inner action will not be changed.
         Else, the timeline of inner action will be changed by:
         \f$-{ 2 }^{ 10*(time-1) }*sin((time-1-\frac { period }{ 4 } )*\pi *2/period)\f$.

 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseElasticIn : public EaseElastic
{
public:
    /** 
     @brief Create the EaseElasticIn action with the inner action and the period in radians.
     @param action The pointer of the inner action.
     @param period Period of the wave in radians.
     @return A pointer of EaseElasticIn action. If creation failed, return nil.
    */
    static EaseElasticIn* create(ActionInterval *action, float period);

    /** 
     @brief Create the EaseElasticIn action with the inner action and period value is 0.3.
     @param action The pointer of the inner action.
     @return A pointer of EaseElasticIn action. If creation failed, return nil.
    */
    static EaseElasticIn* create(ActionInterval *action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseElasticIn* copy();
    virtual EaseElastic* reverse() const override;

public:
    EaseElasticIn() {}
    virtual ~EaseElasticIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseElasticIn);
};

/** 
 @class EaseElasticOut
 @brief Ease Elastic Out action.
 @details If time == 0 or time == 1, the timeline of inner action will not be changed.
         Else, the timeline of inner action will be changed by:
         \f${ 2 }^{ -10*time }*sin((time-\frac { period }{ 4 } )*\pi *2/period)+1\f$.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseElasticOut : public EaseElastic
{
public:
    /** 
     @brief Create the EaseElasticOut action with the inner action and the period in radians.
     @param action The pointer of the inner action.
     @param period Period of the wave in radians.
     @return A pointer of EaseElasticOut action. If creation failed, return nil.
    */
    static EaseElasticOut* create(ActionInterval *action, float period);

    /** 
     @brief Create the EaseElasticOut action with the inner action and period value is 0.3.
     @param action The pointer of the inner action.
     @return A pointer of EaseElasticOut action. If creation failed, return nil.
    */
    static EaseElasticOut* create(ActionInterval *action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseElasticOut* copy();
    virtual EaseElastic* reverse() const override;

public:
    EaseElasticOut() {}
    virtual ~EaseElasticOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseElasticOut);
};

/** 
 @class EaseElasticInOut
 @brief Ease Elastic InOut action.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseElasticInOut : public EaseElastic
{
public:
    /** 
     @brief Create the EaseElasticInOut action with the inner action and the period in radians.
     @param action The pointer of the inner action.
     @param period Period of the wave in radians.
     @return A pointer of EaseElasticInOut action. If creation failed, return nil.
    */
    static EaseElasticInOut* create(ActionInterval *action, float period);

    /** 
     @brief Create the EaseElasticInOut action with the inner action and period value is 0.3.
     @param action The pointer of the inner action.
     @return A pointer of EaseElasticInOut action. If creation failed, return nil.
    */
    static EaseElasticInOut* create(ActionInterval *action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseElasticInOut* copy();
    virtual EaseElasticInOut* reverse() const override;

public:
    EaseElasticInOut() {}
    virtual ~EaseElasticInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseElasticInOut);
};

/** 
 @class EaseBounce
 @brief EaseBounce abstract class.
 @since v0.8.2
 @ingroup Actions
*/
class CC_DLL EaseBounce : public ActionEase
{
public:

    // Overrides
    virtual EaseBounce* copy()
    {
        CC_ASSERT(0);
        return nullptr;
    }

    virtual EaseBounce* reverse() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }

public:
    EaseBounce() {}
    virtual ~EaseBounce() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBounce);
};

/** 
 @class EaseBounceIn
 @brief EaseBounceIn action.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
*/
class CC_DLL EaseBounceIn : public EaseBounce
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseBounceIn action. If creation failed, return nil.
    */
    static EaseBounceIn* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseBounceIn* copy();
    virtual EaseBounce* reverse() const override;

public:
    EaseBounceIn() {}
    virtual ~EaseBounceIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBounceIn);
};

/** 
 @class EaseBounceOut
 @brief EaseBounceOut action.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseBounceOut : public EaseBounce
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseBounceOut action. If creation failed, return nil.
    */
    static EaseBounceOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseBounceOut* copy();
    virtual EaseBounce* reverse() const override;

public:
    EaseBounceOut() {}
    virtual ~EaseBounceOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBounceOut);
};

/** 
 @class EaseBounceInOut
 @brief EaseBounceInOut action.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseBounceInOut : public EaseBounce
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseBounceInOut action. If creation failed, return nil.
    */
    static EaseBounceInOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseBounceInOut* copy();
    virtual EaseBounceInOut* reverse() const override;

public:
    EaseBounceInOut() {}
    virtual ~EaseBounceInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBounceInOut);
};

/** 
 @class EaseBackIn
 @brief EaseBackIn action.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseBackIn : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseBackIn action. If creation failed, return nil.
    */
    static EaseBackIn* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseBackIn* copy();
    virtual ActionEase* reverse() const override;

public:
    EaseBackIn() {}
    virtual ~EaseBackIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBackIn);
};

/** 
 @class EaseBackOut
 @brief EaseBackOut action.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseBackOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseBackOut action. If creation failed, return nil.
    */
    static EaseBackOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseBackOut* copy();
    virtual ActionEase* reverse() const override;

public:
    EaseBackOut() {}
    virtual ~EaseBackOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBackOut);
};

/** 
 @class EaseBackInOut
 @brief EaseBackInOut action.
 @warning This action doesn't use a bijective function.
          Actions like Sequence might have an unexpected result when used with this action.
 @since v0.8.2
 @ingroup Actions
 */
class CC_DLL EaseBackInOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseBackInOut action. If creation failed, return nil.
    */
    static EaseBackInOut* create(ActionInterval* action);

    // Overrides
    virtual void update(float time) override;
    virtual EaseBackInOut* copy();
    virtual EaseBackInOut* reverse() const override;

public:
    EaseBackInOut() {}
    virtual ~EaseBackInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBackInOut);
};


/** 
@class EaseBezierAction
@brief Ease Bezier
@ingroup Actions
*/
class CC_DLL EaseBezierAction : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseBezierAction action. If creation failed, return nil.
    */
    static EaseBezierAction* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseBezierAction* copy();
    virtual EaseBezierAction* reverse() const override;

    /**
    @brief Set the bezier parameters.
    */
    virtual void setBezierParamer( float p0, float p1, float p2, float p3);

public:
    EaseBezierAction() {}
    virtual ~EaseBezierAction() {}

protected:
    float _p0;
    float _p1;
    float _p2;
    float _p3;

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseBezierAction);
};

/** 
@class EaseQuadraticActionIn
@brief Ease Quadratic In
@ingroup Actions
*/
class CC_DLL EaseQuadraticActionIn : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuadraticActionIn action. If creation failed, return nil.
    */
    static EaseQuadraticActionIn* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuadraticActionIn* copy();
    virtual EaseQuadraticActionIn* reverse() const override;

public:
    EaseQuadraticActionIn() {}
    virtual ~EaseQuadraticActionIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuadraticActionIn);

};

/** 
@class EaseQuadraticActionOut
@brief Ease Quadratic Out
@ingroup Actions
*/
class CC_DLL EaseQuadraticActionOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuadraticActionOut action. If creation failed, return nil.
    */
    static EaseQuadraticActionOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuadraticActionOut* copy();
    virtual EaseQuadraticActionOut* reverse() const override;

public:
    EaseQuadraticActionOut() {}
    virtual ~EaseQuadraticActionOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuadraticActionOut);

};

/** 
@class EaseQuadraticActionInOut
@brief Ease Quadratic InOut
@ingroup Actions
*/
class CC_DLL EaseQuadraticActionInOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuadraticActionInOut action. If creation failed, return nil.
    */
    static EaseQuadraticActionInOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuadraticActionInOut* copy();
    virtual EaseQuadraticActionInOut* reverse() const override;

public:
    EaseQuadraticActionInOut() {}
    virtual ~EaseQuadraticActionInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuadraticActionInOut);
};

/** 
@class EaseQuarticActionIn
@brief Ease Quartic In
@ingroup Actions
*/
class CC_DLL EaseQuarticActionIn : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuarticActionIn action. If creation failed, return nil.
    */
    static EaseQuarticActionIn* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuarticActionIn* copy();
    virtual EaseQuarticActionIn* reverse() const override;

public:
    EaseQuarticActionIn() {}
    virtual ~EaseQuarticActionIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuarticActionIn);
};

/** 
@class EaseQuarticActionOut
@brief Ease Quartic Out
@ingroup Actions
*/
class CC_DLL EaseQuarticActionOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuarticActionOut action. If creation failed, return nil.
    */
    static EaseQuarticActionOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuarticActionOut* copy();
    virtual EaseQuarticActionOut* reverse() const override;

public:
    EaseQuarticActionOut() {}
    virtual ~EaseQuarticActionOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuarticActionOut);
};

/** 
@class EaseQuarticActionInOut
@brief Ease Quartic InOut
@ingroup Actions
*/
class CC_DLL EaseQuarticActionInOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuarticActionInOut action. If creation failed, return nil.
    */
    static EaseQuarticActionInOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuarticActionInOut* copy();
    virtual EaseQuarticActionInOut* reverse() const override;

public:
    EaseQuarticActionInOut() {}
    virtual ~EaseQuarticActionInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuarticActionInOut);
};


/** 
@class EaseQuinticActionIn
@brief Ease Quintic In
@ingroup Actions
*/
class CC_DLL EaseQuinticActionIn : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuinticActionIn action. If creation failed, return nil.
    */
    static EaseQuinticActionIn* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuinticActionIn* copy();
    virtual EaseQuinticActionIn* reverse() const override;

public:
    EaseQuinticActionIn() {}
    virtual ~EaseQuinticActionIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuinticActionIn);
};

/** 
@class EaseQuinticActionOut
@brief Ease Quintic Out
@ingroup Actions
*/
class CC_DLL EaseQuinticActionOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuinticActionOut action. If creation failed, return nil.
    */
    static EaseQuinticActionOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuinticActionOut* copy();
    virtual EaseQuinticActionOut* reverse() const override;

public:
    EaseQuinticActionOut() {}
    virtual ~EaseQuinticActionOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuinticActionOut);
};

/** 
@class EaseQuinticActionInOut
@brief Ease Quintic InOut
@ingroup Actions
*/
class CC_DLL EaseQuinticActionInOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseQuinticActionInOut action. If creation failed, return nil.
    */
    static EaseQuinticActionInOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseQuinticActionInOut* copy();
    virtual EaseQuinticActionInOut* reverse() const override;

public:
    EaseQuinticActionInOut() {}
    virtual ~EaseQuinticActionInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseQuinticActionInOut);
};

/** 
@class EaseCircleActionIn
@brief Ease Circle In
@ingroup Actions
*/
class CC_DLL EaseCircleActionIn : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseCircleActionIn action. If creation failed, return nil.
    */
    static EaseCircleActionIn* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseCircleActionIn* copy();
    virtual EaseCircleActionIn* reverse() const override;

public:
    EaseCircleActionIn() {}
    virtual ~EaseCircleActionIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseCircleActionIn);
};

/** 
@class EaseCircleActionOut
@brief Ease Circle Out
@ingroup Actions
*/
class CC_DLL EaseCircleActionOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseCircleActionOut action. If creation failed, return nil.
    */
    static EaseCircleActionOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseCircleActionOut* copy();
    virtual EaseCircleActionOut* reverse() const override;

public:
    EaseCircleActionOut() {}
    virtual ~EaseCircleActionOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseCircleActionOut);
};

/** 
@class EaseCircleActionInOut
@brief Ease Circle InOut
@ingroup Actions
*/
class CC_DLL EaseCircleActionInOut:public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseCircleActionInOut action. If creation failed, return nil.
    */
    static EaseCircleActionInOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseCircleActionInOut* copy();
    virtual EaseCircleActionInOut* reverse() const override;

public:
    EaseCircleActionInOut() {}
    virtual ~EaseCircleActionInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseCircleActionInOut);
};

/** 
@class EaseCubicActionIn
@brief Ease Cubic In
@ingroup Actions
*/
class CC_DLL EaseCubicActionIn:public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseCubicActionIn action. If creation failed, return nil.
    */
    static EaseCubicActionIn* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseCubicActionIn* copy();
    virtual EaseCubicActionIn* reverse() const override;

public:
    EaseCubicActionIn() {}
    virtual ~EaseCubicActionIn() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseCubicActionIn);
};

/** 
@class EaseCubicActionOut
@brief Ease Cubic Out
@ingroup Actions
*/
class CC_DLL EaseCubicActionOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseCubicActionOut action. If creation failed, return nil.
    */
    static EaseCubicActionOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseCubicActionOut* copy();
    virtual EaseCubicActionOut* reverse() const override;

public:
    EaseCubicActionOut() {}
    virtual ~EaseCubicActionOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseCubicActionOut);
};

/** 
@class EaseCubicActionInOut
@brief Ease Cubic InOut
@ingroup Actions
*/
class CC_DLL EaseCubicActionInOut : public ActionEase
{
public:
    /** 
     @brief Create the action with the inner action.
     @param action The pointer of the inner action.
     @return A pointer of EaseCubicActionInOut action. If creation failed, return nil.
    */
    static EaseCubicActionInOut* create(ActionInterval* action);
    
    virtual void update(float time) override;
    virtual EaseCubicActionInOut* copy();
    virtual EaseCubicActionInOut* reverse() const override;

public:
    EaseCubicActionInOut() {}
    virtual ~EaseCubicActionInOut() {}

private:
    M_DISALLOW_COPY_AND_ASSIGN(EaseCubicActionInOut);
};

// end of actions group
/// @}

NS_CC_END

#endif // __ACTION_CCEASE_ACTION_H__
