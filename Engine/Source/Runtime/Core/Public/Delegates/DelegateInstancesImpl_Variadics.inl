// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/*================================================================================
	DelegateInstancesImpl.inl: Inline implementation of delegate bindings.

	This file in re-included in DelegateCombinations.h for EVERY supported delegate signature.
	Every combination of parameter count, return value presence or other function modifier will
	include this file to generate a delegate interface type and implementation type for that signature.

	The types declared in this file are for internal use only. 
================================================================================*/

#pragma once
#include "Delegates/DelegateInstanceInterface_Variadics.h"
#include "UObject/NameTypes.h"

class UFunction;

/* Macros for function parameter and delegate payload lists
 *****************************************************************************/

/**
 * Implements a delegate binding for UFunctions.
 *
 * @params UserClass Must be an UObject derived class.
 */
template <class UserClass, typename FuncType, typename... VarTypes>
class TBaseUFunctionDelegateInstance;

template <class UserClass, typename WrappedRetValType, typename... ParamTypes, typename... VarTypes>
class TBaseUFunctionDelegateInstance<UserClass, WrappedRetValType (ParamTypes...), VarTypes...> : public IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)>
{
	typedef IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)> Super;

	static_assert((CanConvertPointerFromTo<UserClass, const UObjectBase>::Result), "You cannot use UFunction delegates with non UObject classes.");

public:
	typedef typename TUnwrapType<WrappedRetValType>::Type RetValType;

	TBaseUFunctionDelegateInstance(UserClass* InUserObject, const FName& InFunctionName, VarTypes... Vars)
		: FunctionName (InFunctionName)
		, UserObjectPtr(InUserObject)
		, Payload      (Vars...)
	{
		check(InFunctionName != NAME_None);
		
		if (InUserObject != nullptr)
		{
			CachedFunction = UserObjectPtr->FindFunctionChecked(InFunctionName);
		}
	}

	// IDelegateInstance interface

	virtual FName GetFunctionName( ) const override
	{
		return FunctionName;
	}

	virtual const void* GetRawMethodPtr( ) const override
	{
		return nullptr;
	}

	virtual const void* GetRawUserObject( ) const override
	{
		return UserObjectPtr.Get();
	}

	virtual EDelegateInstanceType::Type GetType() const override
	{
		return EDelegateInstanceType::UFunction;
	}

	virtual bool HasSameObject( const void* InUserObject ) const override
	{
		return (UserObjectPtr.Get() == InUserObject);
	}

	virtual bool IsCompactable( ) const override
	{
		return !UserObjectPtr.Get(true);
	}

	virtual bool IsSafeToExecute( ) const override
	{
		return UserObjectPtr.IsValid();
	}

public:

	// IBaseDelegateInstance interface

	virtual Super* CreateCopy( ) override
	{
		return Payload.ApplyAfter(&Create, UserObjectPtr.Get(), GetFunctionName());
	}

	virtual RetValType Execute(ParamTypes... Params) const override
	{
		typedef TPayload<RetValType (ParamTypes..., VarTypes...)> FParmsWithPayload;

		checkSlow(IsSafeToExecute());

		TPlacementNewer<FParmsWithPayload> PayloadAndParams;
		Payload.ApplyAfter(PayloadAndParams, Params...);
		UserObjectPtr->ProcessEvent(CachedFunction, &PayloadAndParams);
		return PayloadAndParams->GetResult();
	}

	virtual bool IsSameFunction( const Super& Other ) const override
	{
		// NOTE: Payload data is not currently considered when comparing delegate instances.
		// See the comment in multi-cast delegate's Remove() method for more information.
		return ((Other.GetType() == EDelegateInstanceType::UFunction) &&
				(Other.GetRawUserObject() == GetRawUserObject()) &&
				(Other.GetFunctionName() == GetFunctionName()));
	}

public:

	/**
	 * Creates a new UFunction delegate binding for the given user object and function name.
	 *
	 * @param InObject The user object to call the function on.
	 * @param InFunctionName The name of the function call.
	 * @return The new delegate.
	 */
	FORCEINLINE static Super* Create(UserClass* InUserObject, const FName& InFunctionName, VarTypes... Vars)
	{
		return new TBaseUFunctionDelegateInstance<UserClass, RetValType (ParamTypes...), VarTypes...>(InUserObject, InFunctionName, Vars...);
	}

	template <typename TupleType, typename IntSeq>
	struct TCreateFromTuple;

	template <typename... TupleArgs, uint32... Indices>
	struct TCreateFromTuple<TTuple<TupleArgs...>, TIntegerSequence<Indices...>>
	{
		static Super* CreateImpl(UserClass* InUserObject, const FName& InFunctionName, const TTuple<TupleArgs...>& Tuple)
		{
			return Create(InUserObject, InFunctionName, Tuple.template Get<Indices>()...);
		}
	};

	template <typename... TupleArgs>
	FORCEINLINE static Super* CreateFromTuple(UserClass* InUserObject, const FName& InFunctionName, const TTuple<TupleArgs...>& Tuple)
	{
		return TCreateFromTuple<TTuple<TupleArgs...>, TMakeIntegerSequence<sizeof...(TupleArgs)>>::CreateImpl(InUserObject, InFunctionName, Tuple);
	}

public:

	// Holds the cached UFunction to call.
	UFunction* CachedFunction;

	// Holds the name of the function to call.
	FName FunctionName;

	// The user object to call the function on.
	TWeakObjectPtr<UserClass> UserObjectPtr;

	TTuple<VarTypes...> Payload;
};

template <class UserClass, typename... ParamTypes, typename... VarTypes>
class TBaseUFunctionDelegateInstance<UserClass, void (ParamTypes...), VarTypes...> : public TBaseUFunctionDelegateInstance<UserClass, TTypeWrapper<void> (ParamTypes...), VarTypes...>
{
	typedef TBaseUFunctionDelegateInstance<UserClass, TTypeWrapper<void> (ParamTypes...), VarTypes...> Super;

public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InUserObject The UObject to call the function on.
	 * @param InFunctionName The name of the function call.
	 */
	TBaseUFunctionDelegateInstance(UserClass* InUserObject, const FName& InFunctionName, VarTypes... Vars)
		: Super(InUserObject, InFunctionName, Vars...)
	{
	}

	virtual bool ExecuteIfSafe(ParamTypes... Params) const override
	{
		if (Super::IsSafeToExecute())
		{
			Super::Execute(Params...);

			return true;
		}

		return false;
	}
};


/* Delegate binding types
 *****************************************************************************/

/**
 * Implements a delegate binding for shared pointer member functions.
 */
template <bool bConst, class UserClass, ESPMode SPMode, typename FuncType, typename... VarTypes>
class TBaseSPMethodDelegateInstance;

template <bool bConst, class UserClass, ESPMode SPMode, typename WrappedRetValType, typename... ParamTypes, typename... VarTypes>
class TBaseSPMethodDelegateInstance<bConst, UserClass, SPMode, WrappedRetValType (ParamTypes...), VarTypes...> : public IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)>
{
	typedef IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)> Super;

public:
	typedef typename TUnwrapType<WrappedRetValType>::Type RetValType;
	typedef typename TMemFunPtrType<bConst, UserClass, RetValType (ParamTypes..., VarTypes...)>::Type FMethodPtr;

	TBaseSPMethodDelegateInstance(const TSharedPtr<UserClass, SPMode>& InUserObject, FMethodPtr InMethodPtr, VarTypes... Vars)
		: UserObject(InUserObject)
		, MethodPtr (InMethodPtr)
		, Payload   (Vars...)
	{
		// NOTE: Shared pointer delegates are allowed to have a null incoming object pointer.  Weak pointers can expire,
		//       an it is possible for a copy of a delegate instance to end up with a null pointer.
		checkSlow(MethodPtr != nullptr);
	}

	// IDelegateInstance interface

	virtual FName GetFunctionName() const override
	{
		return NAME_None;
	}

	virtual const void* GetRawMethodPtr() const override
	{
		return GetRawMethodPtrInternal();
	}

	virtual const void* GetRawUserObject() const override
	{
		return GetRawUserObjectInternal();
	}

	virtual EDelegateInstanceType::Type GetType() const override
	{
		return SPMode == ESPMode::ThreadSafe ? EDelegateInstanceType::ThreadSafeSharedPointerMethod : EDelegateInstanceType::SharedPointerMethod;
	}

	virtual bool HasSameObject(const void* InUserObject) const override
	{
		return UserObject.HasSameObject(InUserObject);
	}

	virtual bool IsSafeToExecute() const override
	{
		return UserObject.IsValid();
	}

public:

	// IBaseDelegateInstance interface

	virtual Super* CreateCopy() override
	{
		return Payload.template ApplyAfter_ExplicitReturnType<Super*>(static_cast<Super*(*)(const TSharedPtr<UserClass, SPMode>&, FMethodPtr, VarTypes...)>(&Create), UserObject.Pin(), MethodPtr);
	}

	virtual RetValType Execute(ParamTypes... Params) const override
	{
		typedef typename TRemoveConst<UserClass>::Type MutableUserClass;

		// Verify that the user object is still valid.  We only have a weak reference to it.
		auto SharedUserObject = UserObject.Pin();
		checkSlow(SharedUserObject.IsValid());

		// Safely remove const to work around a compiler issue with instantiating template permutations for 
		// overloaded functions that take a function pointer typedef as a member of a templated class.  In
		// all cases where this code is actually invoked, the UserClass will already be a const pointer.
		auto MutableUserObject = const_cast<MutableUserClass*>(SharedUserObject.Get());

		// Call the member function on the user's object.  And yes, this is the correct C++ syntax for calling a
		// pointer-to-member function.
		checkSlow(MethodPtr != nullptr);

		return Payload.template ApplyAfter_ExplicitReturnType<RetValType>(TMemberFunctionCaller<MutableUserClass, FMethodPtr>(MutableUserObject, MethodPtr), Params...);
	}

	virtual bool IsSameFunction( const Super& InOtherDelegate ) const override
	{
		// NOTE: Payload data is not currently considered when comparing delegate instances.
		// See the comment in multi-cast delegate's Remove() method for more information.
		if ((InOtherDelegate.GetType() == EDelegateInstanceType::SharedPointerMethod) || 
			(InOtherDelegate.GetType() == EDelegateInstanceType::ThreadSafeSharedPointerMethod) ||
			(InOtherDelegate.GetType() == EDelegateInstanceType::RawMethod))
		{
			return (GetRawMethodPtrInternal() == InOtherDelegate.GetRawMethodPtr() && UserObject.HasSameObject(InOtherDelegate.GetRawUserObject()));
		}

		return false;
	}

public:

	/**
	 * Creates a new shared pointer delegate binding for the given user object and method pointer.
	 *
	 * @param InUserObjectRef Shared reference to the user's object that contains the class method.
	 * @param InFunc Member function pointer to your class method.
	 * @return The new delegate.
	 */
	FORCEINLINE static Super* Create(const TSharedPtr<UserClass, SPMode>& InUserObjectRef, FMethodPtr InFunc, VarTypes... Vars)
	{
		return new TBaseSPMethodDelegateInstance<bConst, UserClass, SPMode, RetValType (ParamTypes...), VarTypes...>(InUserObjectRef, InFunc, Vars...);
	}

	/**
	 * Creates a new shared pointer delegate binding for the given user object and method pointer.
	 *
	 * This overload requires that the supplied object derives from TSharedFromThis.
	 *
	 * @param InUserObject  The user's object that contains the class method.  Must derive from TSharedFromThis.
	 * @param InFunc  Member function pointer to your class method.
	 * @return The new delegate.
	 */
	FORCEINLINE static Super* Create(UserClass* InUserObject, FMethodPtr InFunc, VarTypes... Vars)
	{
		// We expect the incoming InUserObject to derived from TSharedFromThis.
		auto UserObjectRef = StaticCastSharedRef<UserClass>(InUserObject->AsShared());
		return Create(UserObjectRef, InFunc, Vars...);
	}

protected:

	/**
	 * Internal, inlined and non-virtual version of GetRawUserObject interface.
	 */
	FORCEINLINE const void* GetRawUserObjectInternal( ) const
	{
		return UserObject.Pin().Get();
	}

	/**
	 * Internal, inlined and non-virtual version of GetRawMethod interface.
	 */
	FORCEINLINE const void* GetRawMethodPtrInternal( ) const
	{
		return *(const void**)&MethodPtr;
	}

	// Weak reference to an instance of the user's class which contains a method we would like to call.
	TWeakPtr<UserClass, SPMode> UserObject;

	// C++ member function pointer.
	FMethodPtr MethodPtr;

	// Payload member variables, if any.
	TTuple<VarTypes...> Payload;
};

template <bool bConst, class UserClass, ESPMode SPMode, typename... ParamTypes, typename... VarTypes>
class TBaseSPMethodDelegateInstance<bConst, UserClass, SPMode, void (ParamTypes...), VarTypes...> : public TBaseSPMethodDelegateInstance<bConst, UserClass, SPMode, TTypeWrapper<void> (ParamTypes...), VarTypes...>
{
	typedef TBaseSPMethodDelegateInstance<bConst, UserClass, SPMode, TTypeWrapper<void> (ParamTypes...), VarTypes...> Super;

public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InUserObject A shared reference to an arbitrary object (templated) that hosts the member function.
	 * @param InMethodPtr C++ member function pointer for the method to bind.
	 */
	TBaseSPMethodDelegateInstance(const TSharedPtr<UserClass, SPMode>& InUserObject, typename Super::FMethodPtr InMethodPtr, VarTypes... Vars)
		: Super(InUserObject, InMethodPtr, Vars...)
	{
	}

	virtual bool ExecuteIfSafe(ParamTypes... Params) const override
	{
		// Verify that the user object is still valid.  We only have a weak reference to it.
		auto SharedUserObject = Super::UserObject.Pin();
		if (SharedUserObject.IsValid())
		{
			Super::Execute(Params...);

			return true;
		}

		return false;
	}
};


/**
 * Implements a delegate binding for C++ member functions.
 */
template <bool bConst, class UserClass, typename FuncType, typename... VarTypes>
class TBaseRawMethodDelegateInstance;

template <bool bConst, class UserClass, typename WrappedRetValType, typename... ParamTypes, typename... VarTypes>
class TBaseRawMethodDelegateInstance<bConst, UserClass, WrappedRetValType (ParamTypes...), VarTypes...> : public IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)>
{
	static_assert((!CanConvertPointerFromTo<UserClass, const UObjectBase>::Result), "You cannot use raw method delegates with UObjects.");

	typedef IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)> Super;

public:
	typedef typename TUnwrapType<WrappedRetValType>::Type RetValType;
	typedef typename TMemFunPtrType<bConst, UserClass, RetValType (ParamTypes..., VarTypes...)>::Type FMethodPtr;

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InUserObject An arbitrary object (templated) that hosts the member function.
	 * @param InMethodPtr C++ member function pointer for the method to bind.
	 */
	TBaseRawMethodDelegateInstance(UserClass* InUserObject, FMethodPtr InMethodPtr, VarTypes... Vars)
		: UserObject(InUserObject)
		, MethodPtr (InMethodPtr)
		, Payload   (Vars...)
	{
		// Non-expirable delegates must always have a non-null object pointer on creation (otherwise they could never execute.)
		check(InUserObject != nullptr && MethodPtr != nullptr);
	}

	// IDelegateInstance interface

	virtual FName GetFunctionName( ) const override
	{
		return NAME_None;
	}

	virtual const void* GetRawMethodPtr( ) const override
	{
		return GetRawMethodPtrInternal();
	}

	virtual const void* GetRawUserObject( ) const override
	{
		return GetRawUserObjectInternal();
	}

	virtual EDelegateInstanceType::Type GetType( ) const override
	{
		return EDelegateInstanceType::RawMethod;
	}

	virtual bool HasSameObject( const void* InUserObject ) const override
	{
		return UserObject == InUserObject;
	}

	virtual bool IsSafeToExecute( ) const override
	{
		// We never know whether or not it is safe to deference a C++ pointer, but we have to
		// trust the user in this case.  Prefer using a shared-pointer based delegate type instead!
		return true;
	}

public:

	// IBaseDelegateInstance interface

	virtual Super* CreateCopy( ) override
	{
		return Payload.template ApplyAfter_ExplicitReturnType<Super*>(&Create, UserObject, MethodPtr);
	}

	virtual RetValType Execute(ParamTypes... Params) const override
	{
		typedef typename TRemoveConst<UserClass>::Type MutableUserClass;

		// Safely remove const to work around a compiler issue with instantiating template permutations for 
		// overloaded functions that take a function pointer typedef as a member of a templated class.  In
		// all cases where this code is actually invoked, the UserClass will already be a const pointer.
		auto MutableUserObject = const_cast<MutableUserClass*>(UserObject);

		// Call the member function on the user's object.  And yes, this is the correct C++ syntax for calling a
		// pointer-to-member function.
		checkSlow(MethodPtr != nullptr);

		return Payload.template ApplyAfter_ExplicitReturnType<RetValType>(TMemberFunctionCaller<MutableUserClass, FMethodPtr>(MutableUserObject, MethodPtr), Params...);
	}

	virtual bool IsSameFunction( const Super& InOtherDelegate ) const override
	{
		// NOTE: Payload data is not currently considered when comparing delegate instances.
		// See the comment in multi-cast delegate's Remove() method for more information.
		if ((InOtherDelegate.GetType() == EDelegateInstanceType::RawMethod) || 
			(InOtherDelegate.GetType() == EDelegateInstanceType::UObjectMethod) ||
			(InOtherDelegate.GetType() == EDelegateInstanceType::SharedPointerMethod) ||
			(InOtherDelegate.GetType() == EDelegateInstanceType::ThreadSafeSharedPointerMethod))
		{
			return (GetRawMethodPtrInternal() == InOtherDelegate.GetRawMethodPtr()) && (UserObject == InOtherDelegate.GetRawUserObject());
		}

		return false;
	}

public:

	/**
	 * Creates a new raw method delegate binding for the given user object and function pointer.
	 *
	 * @param InUserObject User's object that contains the class method.
	 * @param InFunc Member function pointer to your class method.
	 * @return The new delegate.
	 */
	FORCEINLINE static Super* Create(UserClass* InUserObject, FMethodPtr InFunc, VarTypes... Vars)
	{
		return new TBaseRawMethodDelegateInstance<bConst, UserClass, RetValType (ParamTypes...), VarTypes...>(InUserObject, InFunc, Vars...);
	}

protected:

	/**
	 * Internal, inlined and non-virtual version of GetRawUserObject interface.
	 */
	FORCEINLINE const void* GetRawUserObjectInternal( ) const
	{
		return UserObject;
	}

	/**
	 * Internal, inlined and non-virtual version of GetRawMethodPtr interface.
	 */
	FORCEINLINE const void* GetRawMethodPtrInternal( ) const
	{
		return *(const void**)&MethodPtr;
	}

	// Pointer to the user's class which contains a method we would like to call.
	UserClass* UserObject;

	// C++ member function pointer.
	FMethodPtr MethodPtr;

	// Payload member variables (if any).
	TTuple<VarTypes...> Payload;
};

template <bool bConst, class UserClass, typename... ParamTypes, typename... VarTypes>
class TBaseRawMethodDelegateInstance<bConst, UserClass, void (ParamTypes...), VarTypes...> : public TBaseRawMethodDelegateInstance<bConst, UserClass, TTypeWrapper<void> (ParamTypes...), VarTypes...>
{
	typedef TBaseRawMethodDelegateInstance<bConst, UserClass, TTypeWrapper<void> (ParamTypes...), VarTypes...> Super;

public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InUserObject An arbitrary object (templated) that hosts the member function.
	 * @param InMethodPtr C++ member function pointer for the method to bind.
	 */
	TBaseRawMethodDelegateInstance(UserClass* InUserObject, typename Super::FMethodPtr InMethodPtr, VarTypes... Vars)
		: Super(InUserObject, InMethodPtr, Vars...)
	{
	}

	virtual bool ExecuteIfSafe(ParamTypes... Params) const override
	{
		// We never know whether or not it is safe to deference a C++ pointer, but we have to
		// trust the user in this case.  Prefer using a shared-pointer based delegate type instead!
		Super::Execute(Params...);

		return true;
	}
};


/**
 * Implements a delegate binding for UObject methods.
 */
template <bool bConst, class UserClass, typename FuncType, typename... VarTypes>
class TBaseUObjectMethodDelegateInstance;

template <bool bConst, class UserClass, typename WrappedRetValType, typename... ParamTypes, typename... VarTypes>
class TBaseUObjectMethodDelegateInstance<bConst, UserClass, WrappedRetValType (ParamTypes...), VarTypes...> : public IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)>
{
	typedef IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)> Super;

	static_assert((CanConvertPointerFromTo<UserClass, const UObjectBase>::Result), "You cannot use UObject method delegates with raw pointers.");

public:
	typedef typename TUnwrapType<WrappedRetValType>::Type RetValType;
	typedef typename TMemFunPtrType<bConst, UserClass, RetValType (ParamTypes..., VarTypes...)>::Type FMethodPtr;

	TBaseUObjectMethodDelegateInstance(UserClass* InUserObject, FMethodPtr InMethodPtr, VarTypes... Vars)
		: UserObject(InUserObject)
		, MethodPtr (InMethodPtr)
		, Payload   (Vars...)
	{
		// NOTE: UObject delegates are allowed to have a null incoming object pointer.  UObject weak pointers can expire,
		//       an it is possible for a copy of a delegate instance to end up with a null pointer.
		checkSlow(MethodPtr != nullptr);
	}

	// IDelegateInstance interface

	virtual FName GetFunctionName( ) const override
	{
		return NAME_None;
	}

	virtual const void* GetRawMethodPtr( ) const override
	{
		return GetRawMethodPtrInternal();
	}

	virtual const void* GetRawUserObject( ) const override
	{
		return GetRawUserObjectInternal();
	}

	virtual EDelegateInstanceType::Type GetType( ) const override
	{
		return EDelegateInstanceType::UObjectMethod;
	}

	virtual bool HasSameObject( const void* InUserObject ) const override
	{
		return (UserObject.Get() == InUserObject);
	}

	virtual bool IsCompactable( ) const override
	{
		return !UserObject.Get(true);
	}

	virtual bool IsSafeToExecute( ) const override
	{
		return !!UserObject.Get();
	}

public:

	// IBaseDelegateInstance interface

	virtual Super* CreateCopy( ) override
	{
		return Payload.template ApplyAfter_ExplicitReturnType<Super*>(&Create, UserObject.Get(), MethodPtr);
	}

	virtual RetValType Execute(ParamTypes... Params) const override
	{
		typedef typename TRemoveConst<UserClass>::Type MutableUserClass;

		// Verify that the user object is still valid.  We only have a weak reference to it.
		checkSlow(UserObject.IsValid());

		// Safely remove const to work around a compiler issue with instantiating template permutations for 
		// overloaded functions that take a function pointer typedef as a member of a templated class.  In
		// all cases where this code is actually invoked, the UserClass will already be a const pointer.
		auto MutableUserObject = const_cast<MutableUserClass*>(UserObject.Get());

		// Call the member function on the user's object.  And yes, this is the correct C++ syntax for calling a
		// pointer-to-member function.
		checkSlow(MethodPtr != nullptr);

		return Payload.template ApplyAfter_ExplicitReturnType<RetValType>(TMemberFunctionCaller<MutableUserClass, FMethodPtr>(MutableUserObject, MethodPtr), Params...);
	}

	virtual bool IsSameFunction( const Super& InOtherDelegate ) const override
	{
		// NOTE: Payload data is not currently considered when comparing delegate instances.
		// See the comment in multi-cast delegate's Remove() method for more information.
		if ((InOtherDelegate.GetType() == EDelegateInstanceType::UObjectMethod) || 
			(InOtherDelegate.GetType() == EDelegateInstanceType::RawMethod))
		{
			return (GetRawMethodPtrInternal() == InOtherDelegate.GetRawMethodPtr()) && (UserObject.Get() == InOtherDelegate.GetRawUserObject());
		}

		return false;
	}

public:

	/**
	 * Creates a new UObject delegate binding for the given user object and method pointer.
	 *
	 * @param InUserObject User's object that contains the class method.
	 * @param InFunc Member function pointer to your class method.
	 * @return The new delegate.
	 */
	FORCEINLINE static Super* Create(UserClass* InUserObject, FMethodPtr InFunc, VarTypes... Vars)
	{
		return new TBaseUObjectMethodDelegateInstance<bConst, UserClass, RetValType (ParamTypes...), VarTypes...>(InUserObject, InFunc, Vars...);
	}

protected:

	/**
	 * Internal, inlined and non-virtual version of GetRawUserObject interface.
	 */
	FORCEINLINE const void* GetRawUserObjectInternal( ) const
	{
		return UserObject.Get();
	}

	/**
	 * Internal, inlined and non-virtual version of GetRawMethodPtr interface.
	 */
	FORCEINLINE const void* GetRawMethodPtrInternal( ) const
	{
		return *(const void**)&MethodPtr;
	}

	// Pointer to the user's class which contains a method we would like to call.
	TWeakObjectPtr<UserClass> UserObject;

	// C++ member function pointer.
	FMethodPtr MethodPtr;

	// Payload member variables (if any).
	TTuple<VarTypes...> Payload;
};

template <bool bConst, class UserClass, typename... ParamTypes, typename... VarTypes>
class TBaseUObjectMethodDelegateInstance<bConst, UserClass, void (ParamTypes...), VarTypes...> : public TBaseUObjectMethodDelegateInstance<bConst, UserClass, TTypeWrapper<void> (ParamTypes...), VarTypes...>
{
	typedef TBaseUObjectMethodDelegateInstance<bConst, UserClass, TTypeWrapper<void> (ParamTypes...), VarTypes...> Super;

public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InUserObject An arbitrary object (templated) that hosts the member function.
	 * @param InMethodPtr C++ member function pointer for the method to bind.
	 */
	TBaseUObjectMethodDelegateInstance(UserClass* InUserObject, typename Super::FMethodPtr InMethodPtr, VarTypes... Vars)
		: Super(InUserObject, InMethodPtr, Vars...)
	{
	}

	virtual bool ExecuteIfSafe(ParamTypes... Params) const override
	{
		// Verify that the user object is still valid.  We only have a weak reference to it.
		auto ActualUserObject = Super::UserObject.Get();
		if (ActualUserObject != nullptr)
		{
			Super::Execute(Params...);

			return true;
		}
		return false;
	}
};


/**
 * Implements a delegate binding for regular C++ functions.
 */
template <typename FuncType, typename... VarTypes>
class TBaseStaticDelegateInstance;

template <typename WrappedRetValType, typename... ParamTypes, typename... VarTypes>
class TBaseStaticDelegateInstance<WrappedRetValType (ParamTypes...), VarTypes...> : public IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)>
{
	typedef IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type (ParamTypes...)> Super;

public:
	typedef typename TUnwrapType<WrappedRetValType>::Type RetValType;
	typedef RetValType (*FFuncPtr)(ParamTypes..., VarTypes...);

	TBaseStaticDelegateInstance(FFuncPtr InStaticFuncPtr, VarTypes... Vars)
		: StaticFuncPtr(InStaticFuncPtr)
		, Payload      (Vars...)
	{
		check(StaticFuncPtr != nullptr);
	}

	// IDelegateInstance interface

	virtual FName GetFunctionName( ) const override
	{
		return NAME_None;
	}

	virtual const void* GetRawMethodPtr( ) const override
	{
		return *(const void**)&StaticFuncPtr;
	}

	virtual const void* GetRawUserObject( ) const override
	{
		return nullptr;
	}

	virtual EDelegateInstanceType::Type GetType( ) const override
	{
		return EDelegateInstanceType::Raw;
	}

	virtual bool HasSameObject( const void* UserObject ) const override
	{
		// Raw Delegates aren't bound to an object so they can never match
		return false;
	}

	virtual bool IsSafeToExecute( ) const override
	{
		// Static functions are always safe to execute!
		return true;
	}

public:

	// IBaseDelegateInstance interface

	virtual Super* CreateCopy( ) override
	{
		return Payload.template ApplyAfter_ExplicitReturnType<Super*>(&Create, StaticFuncPtr);
	}

	virtual RetValType Execute(ParamTypes... Params) const override
	{
		// Call the static function
		checkSlow(StaticFuncPtr != nullptr);

		return Payload.template ApplyAfter_ExplicitReturnType<RetValType>(StaticFuncPtr, Params...);
	}

	virtual bool IsSameFunction( const Super& InOtherDelegate ) const override
	{
		// NOTE: Payload data is not currently considered when comparing delegate instances.
		// See the comment in multi-cast delegate's Remove() method for more information.
		if (InOtherDelegate.GetType() == EDelegateInstanceType::Raw)
		{
			// Downcast to our delegate type and compare
			const auto& OtherStaticDelegate = static_cast<const TBaseStaticDelegateInstance<RetValType (ParamTypes...), VarTypes...>&>(InOtherDelegate);

			return (StaticFuncPtr == OtherStaticDelegate.StaticFuncPtr);
		}

		return false;
	}

public:

	/**
	 * Creates a new static function delegate binding for the given function pointer.
	 *
	 * @param InFunc Static function pointer.
	 * @return The new delegate.
	 */
	FORCEINLINE static Super* Create(FFuncPtr InFunc, VarTypes... Vars)
	{
		return new TBaseStaticDelegateInstance<RetValType (ParamTypes...), VarTypes...>(InFunc, Vars...);
	}

private:

	// C++ function pointer.
	FFuncPtr StaticFuncPtr;

	// Payload member variables, if any.
	TTuple<VarTypes...> Payload;
};

template <typename... ParamTypes, typename... VarTypes>
class TBaseStaticDelegateInstance<void (ParamTypes...), VarTypes...> : public TBaseStaticDelegateInstance<TTypeWrapper<void> (ParamTypes...), VarTypes...>
{
	typedef TBaseStaticDelegateInstance<TTypeWrapper<void> (ParamTypes...), VarTypes...> Super;

public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InStaticFuncPtr C++ function pointer.
	 */
	TBaseStaticDelegateInstance(typename Super::FFuncPtr InStaticFuncPtr, VarTypes... Vars)
		: Super(InStaticFuncPtr, Vars...)
	{
	}

	virtual bool ExecuteIfSafe(ParamTypes... Params) const override
	{
		Super::Execute(Params...);

		return true;
	}
};

/**
 * Implements a delegate binding for C++ functors, e.g. lambdas.
 */
template <typename FuncType, typename FunctorType, typename... VarTypes>
class TBaseFunctorDelegateInstance;

template <typename WrappedRetValType, typename... ParamTypes, typename FunctorType, typename... VarTypes>
class TBaseFunctorDelegateInstance<WrappedRetValType(ParamTypes...), FunctorType, VarTypes...> : public IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type(ParamTypes...)>
{
	static_assert(TAreTypesEqual<FunctorType, typename TRemoveReference<FunctorType>::Type>::Value, "FunctorType cannot be a reference");

	typedef IBaseDelegateInstance<typename TUnwrapType<WrappedRetValType>::Type(ParamTypes...)> Super;

public:
	typedef typename TUnwrapType<WrappedRetValType>::Type RetValType;

	TBaseFunctorDelegateInstance(const FunctorType& InFunctor, VarTypes... Vars)
		: Functor(InFunctor)
		, Payload(Vars...)
	{
	}
	TBaseFunctorDelegateInstance(FunctorType&& InFunctor, VarTypes... Vars)
		: Functor(MoveTemp(InFunctor))
		, Payload(Vars...)
	{
	}

	// IDelegateInstance interface

	virtual FName GetFunctionName() const override
	{
		return NAME_None;
	}

	virtual const void* GetRawMethodPtr() const override
	{
		// casting operator() to void* is not legal C++ if it's a member function
		// and wouldn't necessarily be a useful thing to return anyway
		check(0);
		return nullptr;
	}

	virtual const void* GetRawUserObject() const override
	{
		// returning &Functor wouldn't be particularly useful to the comparison code
		// as it would always be unique because we store a copy of the functor
		check(0);
		return nullptr;
	}

	virtual EDelegateInstanceType::Type GetType() const override
	{
		return EDelegateInstanceType::Functor;
	}

	virtual bool HasSameObject(const void* UserObject) const override
	{
		// Functor Delegates aren't bound to a user object so they can never match
		return false;
	}

	virtual bool IsSafeToExecute() const override
	{
		// Functors are always considered safe to execute!
		return true;
	}

public:
	// IBaseDelegateInstance interface
	virtual Super* CreateCopy() override
	{
		return Payload.template ApplyAfter_ExplicitReturnType<Super*>(static_cast<Super* (*)(const FunctorType&)>(&Create), Functor);
	}

	virtual RetValType Execute(ParamTypes... Params) const override
	{
		return Payload.template ApplyAfter_ExplicitReturnType<RetValType>(Functor, Params...);
	}

	virtual bool IsSameFunction(const Super& InOtherDelegate) const override
	{
		// There's no nice way to implement this (we don't have the type info necessary to compare against OtherDelegate's Functor)
		return false;
	}

public:
	/**
	 * Creates a new static function delegate binding for the given function pointer.
	 *
	 * @param InFunctor C++ functor
	 * @return The new delegate.
	 */
	FORCEINLINE static Super* Create(const FunctorType& InFunctor, VarTypes... Vars)
	{
		return new TBaseFunctorDelegateInstance<RetValType(ParamTypes...), FunctorType, VarTypes...>(InFunctor, Vars...);
	}
	FORCEINLINE static Super* Create(FunctorType&& InFunctor, VarTypes... Vars)
	{
		return new TBaseFunctorDelegateInstance<RetValType(ParamTypes...), FunctorType, VarTypes...>(MoveTemp(InFunctor), Vars...);
	}

private:
	// C++ functor
	FunctorType Functor;

	// Payload member variables, if any.
	TTuple<VarTypes...> Payload;
};

template <typename FunctorType, typename... ParamTypes, typename... VarTypes>
class TBaseFunctorDelegateInstance<void(ParamTypes...), FunctorType, VarTypes...> : public TBaseFunctorDelegateInstance<TTypeWrapper<void>(ParamTypes...), FunctorType, VarTypes...>
{
	typedef TBaseFunctorDelegateInstance<TTypeWrapper<void>(ParamTypes...), FunctorType, VarTypes...> Super;

public:
	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InFunctor C++ functor
	 */
	TBaseFunctorDelegateInstance(const FunctorType& InFunctor, VarTypes... Vars)
		: Super(InFunctor, Vars...)
	{
	}
	TBaseFunctorDelegateInstance(FunctorType&& InFunctor, VarTypes... Vars)
		: Super(MoveTemp(InFunctor), Vars...)
	{
	}

	virtual bool ExecuteIfSafe(ParamTypes... Params) const override
	{
		// Functors are always considered safe to execute!
		Super::Execute(Params...);

		return true;
	}
};
