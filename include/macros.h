/*
 * common.h
 *
 *  Created on: May 28, 2020
 *      Author: local-admin
 */

#pragma once

#define DEBUG 1

const int IMAGE_WIDTH = 2592;
const int IMAGE_HEIGHT = 1944;




/*
 * Create get<funName>/set<funName> for instance varable of
 * type <varType> and name <varName> with initial value v
 *
 */
#define GETTERSETTER(varType, varName, funName)\
    protected: varType varName ;\
    public: virtual varType get##funName(void) const {\
        return varName;\
    }\
    public: virtual void set##funName(varType v){\
        varName =v;\
    }\

#define ATTRV(varType, varName, funName, v)\
    protected: varType varName = v ;\
    public: virtual varType get##funName(void) const {\
        return varName;\
    }\
    public: virtual void set##funName(varType var){\
        varName = var;\
    }\


/**
 * Creats get<funName>/set<funName> for instance varable of
 * type <varType> and name <varName> with initial value v
 *
 */
#define ATTR(varType, varName, funName, v)\
    protected: varType varName = v ;\
    public: virtual varType get##funName(void) const {\
        return varName;\
    }\
    public: virtual void set##funName(varType var){\
        varName = var;\
    }\

#define GETTER_SETTER(varType, varName, funName)\
    protected: varType varName  ;\
    public: virtual varType get##funName(void) const {\
        return varName;\
    }\
    public: virtual void set##funName(varType var){\
        varName = var;\
    }\


#define GETTER(varType, varName, funName)\
protected: varType varName  ;\
public: virtual varType get##funName(void) const {\
return varName;\
}\


/**
 * Creats static get<funName>/set<funName> for static varable of
 * type <varType> and name <varName> with initial value v
 *
 */
#define STATIC_ATTR(varType, varName, funName, v)\
    protected: static varType varName = v ;\
    public: static varType get##funName(void) const {\
    return varName;\
}\
public: static void set##funName(varType var){\
    varName = var;\
}\






