import React from 'react';
import PropTypes from "prop-types";
import { Button, Modal as BaseModal, ModalHeader, ModalBody, ModalFooter } from 'reactstrap';

const Modal = props => {
  const {
    modalTitle,
    triggerBtnLabel,
    triggerBtnColor,
    actionBtnLabel,
    className,
    onAction,
    modal,
    onToggle
  } = props;

  return (
      <div>
          <div className={className}>
            <Button color={triggerBtnColor}
                    onClick={onToggle}
                    className="mx-2"
                    type="button"
            >
              {triggerBtnLabel}
            </Button>
          </div>
      <BaseModal isOpen={modal} toggle={onToggle} className={className}>
          <ModalHeader toggle={onToggle}>{modalTitle}</ModalHeader>
          <ModalBody>
            {props.children}
          </ModalBody>
          <ModalFooter>
            <Button color="success" type="button" onClick={onAction}>{actionBtnLabel}</Button>{' '}
            <Button color="secondary" type="reset" onClick={onToggle}>Cancel</Button>
          </ModalFooter>
        </BaseModal>
      </div>
  );
};

Modal.propTypes = {
    triggerBtnLabel: PropTypes.string.isRequired,
    actionBtnLabel: PropTypes.string.isRequired,
    onAction: PropTypes.func.isRequired,
    onToggle: PropTypes.func.isRequired,
    triggerBtnColor: PropTypes.string,
    modalTitle: PropTypes.string,
    className: PropTypes.string,
    modal: PropTypes.bool,
};

Modal.defaultProps = {
    modalTitle: '',
    className: '',
    triggerBtnColor: 'secondary',
    modal: false,
};

export default Modal;